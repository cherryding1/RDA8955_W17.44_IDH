
#include "vppp_speech_common.h"


// VoC_directive: PARSER_OFF

#include <stdio.h>
extern FILE *TestFileHandle;
extern FILE *OutFileHandle;
extern FILE *LogFileHandle;
extern FILE *AecSpkFileHandle;
extern FILE *AecDelaySpkFileHandle;
extern FILE *OutSynFilterDelayLine_fileid;
extern FILE *FilterBankMulBuf_fileid;
extern FILE *AFUpdateItem_fileid;
extern FILE *DebugOverflow_fileid;


static int TempFixI, TempFixQ;

#define AEC_VOC_REF_DEBUG
#define AEC_VOC_PF_DEBUG
#define AEC_VOC_SYN_DEBUG
#define AEC_VOC_AF_DEBUG
#define AEC_VOC_ECHO_DEBUG

#define OverflowPrint(line, msg) \
		do {\
			fprintf(DebugOverflow_fileid, "OVERFLOW: line %d: ", line);\
			fprintf(DebugOverflow_fileid, "%s\n", msg);\
		} while (0)

// VoC_directive: PARSER_ON

#if 0

	voc_short For_AEC_align	,816,x


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


	voc_short r_SpkAnaFilterDelayLine[PROTOTYPE_FILTER_LEN],x					// spk Analysis Filter bank Delay Line
	voc_short r_MicAnaFilterDelayLine[PROTOTYPE_FILTER_LEN],x					// mic Analysis Filter bank Delay Line
	
	// r_OutSynFilterDelayLine is Q17, we should use voc_word
	voc_word r_OutSynFilterDelayLine[PROTOTYPE_FILTER_LEN],x					// Out Synthesis Filter bank Delay Line
	
	// g_FilterBank_MulBuf is Q17, but the max range is 0.25, so we could store it in voc_short
	voc_short g_FilterBank_MulBuf[PROTOTYPE_FILTER_LEN],x						// Filter Bank Inlternal Mul buffer

	voc_short g_SpkAnaFilterOutBufI[SUBBAND_CHANNEL_NUM],x						// Spk Analysis Filter Output Buffer
	voc_short g_SpkAnaFilterOutBufQ[SUBBAND_CHANNEL_NUM],x						// Spk Analysis Filter Output Buffer
	voc_short g_MicAnaFilterOutBufI[SUBBAND_CHANNEL_NUM],x						// Mic Analysis Filter Output Buffer
	voc_short g_MicAnaFilterOutBufQ[SUBBAND_CHANNEL_NUM],x						// Mic Analysis Filter Output Buffer

	voc_short g_OutSynFilterInBufI[SUBBAND_CHANNEL_NUM],x						// Out Synthesis Filter Input Buffer
	voc_short g_OutSynFilterInBufQ[SUBBAND_CHANNEL_NUM],x						// Out Synthesis Filter Input Buffer

	

	voc_short r_AF_SpkSigBufI[SUBBANDFILTER_NUM*FILTER_LEN],x					// Spk Sig Buffer for Adaptive Filter
	voc_short r_AF_SpkSigBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x					// Spk Sig Buffer for Adaptive Filter


	voc_word r_AF_FilterBufI[SUBBANDFILTER_NUM*FILTER_LEN],x					// Filter Buf for Adaptive Filter
	voc_word r_AF_FilterBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x					// Filter Buf for Adaptive Filter

	
	voc_short g_AF_FilterOutBufI[SUBBANDFILTER_NUM],x							// Adaptive Filter Output
	voc_short reserve00_addr,x
	voc_short g_AF_FilterOutBufQ[SUBBANDFILTER_NUM],x							// Adaptive Filter Output
	voc_short reserve01_addr,x
	

	voc_short r_AF_OldSpkDataBufI[SUBBANDFILTER_NUM],x							// Old Spk Data for Adatpive Filter (AF)
	voc_short reserve02_addr,x
	voc_short r_AF_OldSpkDataBufQ[SUBBANDFILTER_NUM],x							// Old Spk Data for Adatpive Filter (AF)	
	voc_short reserve03_addr,x

	voc_short g_AF_ErrDataBufI[SUBBANDFILTER_NUM],x								// Err Data Buf for AF
	voc_short reserve04_addr,x
	voc_short g_AF_ErrDataBufQ[SUBBANDFILTER_NUM],x								// Err Data Buf for AF
	voc_short reserve05_addr,x

	voc_short g_AF_ShortFilterBufI[FILTER_LEN],x								// Short Buffer
	voc_short g_AF_ShortFilterBufQ[FILTER_LEN],x								// Short Buffer

	voc_word g_AF_tempCmplxBufI[FILTER_LEN],x									// Temp Complex Buffer
	voc_word g_AF_tempCmplxBufQ[FILTER_LEN],x									// Temp Complex Buffer


	voc_word r_AF_SpkSigPower[SUBBANDFILTER_NUM],x								// Spk Sig Power for Adaptive Filter
	voc_word r_AF_SpkSigSmoothPower[SUBBANDFILTER_NUM],x						// Spk Sig Smooth Power for Adaptive Filter

	voc_word r_AF_RErrSpkI[SUBBANDFILTER_NUM],x									// Correlation between Mic and Spk for Adaptive Filter
	voc_word r_AF_RErrSpkQ[SUBBANDFILTER_NUM],x		    						// Correlation between Mic and Spk for Adaptive Filter


    voc_short g_AF_StepSize[SUBBANDFILTER_NUM],x								// Adaptive Step Size
	voc_short reserve06_addr,x	


	voc_word r_PF_SmoothErrPower[SUBBANDFILTER_NUM],x							// Smooth Err Power
	voc_word r_PF_NoisePowerEstBuf[SUBBANDFILTER_NUM],x							// Noise Power Est Buffer for Post Filter (PF)

	voc_short r_PF_NoiseFloorDevCnt[SUBBANDFILTER_NUM],x						// Noise Floor Deviation Count (PF)
	voc_short g_DTD_ch,x	

	voc_short g_PF_PostSToNRatio[SUBBANDFILTER_NUM],x			// Post SNR for PF
	voc_short CLChnl,x	


	voc_word r_PF_PrevSigPowerNBuf[SUBBANDFILTER_NUM],x		// Prev Sig Power for Noise Suppression (PF)
	voc_word r_PF_PrevSigPowerEBuf[SUBBANDFILTER_NUM],x	    // Prev Sig Power for Echo Suppression (PF)

	voc_short g_PF_NoiseWeight[SUBBANDFILTER_NUM],x				// Noise Weight
	// Echo Canceller Registers
	voc_short r_EC_ErrToOutRatio,x		// Err to Out Ratio
	

	voc_short g_PF_NoiseAmpWeight[SUBBANDFILTER_NUM],x			// Noise Amp Weight
	voc_short m_AF_FrameCnt,x

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

//constant tables

	voc_short g_counter_subframe,x
	voc_short g_ch,x

	voc_word real_int,16,x
	voc_word imag_int,16,x
	
//AEC constant tables
	voc_short AnaFilterCoef[PROTOTYPE_FILTER_LEN],x
	voc_short SynFilterCoef[PROTOTYPE_FILTER_LEN],x
	voc_short wnreal[8],x
	voc_short wnimag[8],x
	voc_short AdaptTable[32],x	

//add new
	voc_word r_PF_CL_MicSmoothPower[PF_CL_CHNLNUM],x							// Mic Smooth Power for PF Control Logic
	voc_word r_PF_CL_MicNoise[PF_CL_CHNLNUM],x									// Mic Noise for PF Control Logic
	voc_word r_PF_CL_SpkSmoothPower[PF_CL_CHNLNUM],x							// Spk Smooth Power for PF Control Logic
	voc_word r_PF_CL_SpkNoise[PF_CL_CHNLNUM],x									// Spk Noise for PF Control Logic

	voc_short r_PF_CL_MicNoiseFloorDevCnt[PF_CL_CHNLNUM],x						// Mic Noise Floor Deviation count for PF Control Logic
	voc_short r_PF_CL_SpkNoiseFloorDevCnt[PF_CL_CHNLNUM],x						// Spk Noise Floor Deviation count for PF Control Logic
	
	voc_short r_PF_CL_ifMicSigOn[PF_CL_CHNLNUM],x								// if Sig on state for PF Control Logic
	voc_short r_PF_CL_ifSpkSigOn[PF_CL_CHNLNUM],x								// if Sig on state for PF Control Logic
	voc_short r_PF_CL_ChnlState[PF_CL_CHNLNUM],x								// State for PF control logic
	voc_short r_PF_CL_State,x													// State for PF control logic
    voc_short r_PF_CL_DTCountDown,x												// DT Count Down Reg for PF Control Logic


	voc_word r_PF_NoisePowerSmoothEstBuf[SUBBANDFILTER_NUM],x					// Noise Power Est Buffer for Post Filter (PF)

	voc_short g_PF_PriorSToNRatio[SUBBANDFILTER_NUM],x
	voc_short g_PF_PriorSToERatio[SUBBANDFILTER_NUM],x							// Prior SER for PF
	voc_word r_PF_SpkPowerBuf[SUBBANDFILTER_NUM],x								// Spk Power Buf for PF


	voc_word r_PF_CCErrSpkVecBufI[SUBBANDFILTER_NUM*FILTER_LEN],x				// Cross Corr between Err and Spk (PF) Vec
	voc_word r_PF_CCErrSpkVecBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x				// Cross Corr between Err and Spk (PF) Vec

	
	voc_word r_PF_EchoPowerEstBuf[SUBBANDFILTER_NUM],x							// Echo Power Est Buffer for Post Filter (PF)

	voc_short g_PF_EchoWeight[SUBBANDFILTER_NUM],x								// Echo Weight

	voc_short r_PF_CL_DTCountUp,x	


	voc_word r_PF_CL_ErrSmoothPower[PF_CL_CHNLNUM],x							// Spk Smooth Power for PF Control Logic
	voc_short r_AF_FilterAmpI[SUBBANDFILTER_NUM],x								// Filter Amp I
	voc_short r_AF_FilterAmpQ[SUBBANDFILTER_NUM],x								// Filter Amp Q





	voc_word r_PF_PrevEchoPowerEst[SUBBANDFILTER_NUM],x							// Prev Echo Power Est for Post Filter (PF)
	voc_word r_PF_PrevSpkPower[SUBBANDFILTER_NUM],x								// Prev Spk Power Est for Post Filter (PF)


	voc_word  r_PF_CL_DTDXcorrMicErrI[PF_CL_CHNLNUM],x							// Xcorr Mic and Err for DTD
	voc_word  r_PF_CL_DTDXcorrMicErrQ[PF_CL_CHNLNUM],x							// Xcorr Mic and Err for DTD
	voc_word  r_PF_CL_DTDXcorrMicEchoI[PF_CL_CHNLNUM],x							// Xcorr Mic and Echo for DTD
	voc_word  r_PF_CL_DTDXcorrMicEchoQ[PF_CL_CHNLNUM],x							// Xcorr Mic and Echo for DTD
	voc_word  r_PF_CL_DTDMicSmoothPower[PF_CL_CHNLNUM],x						// Micpower for DTD
	voc_word  r_PF_CL_DTDErrSmoothPower[PF_CL_CHNLNUM],x						// Errpower for DTD
	voc_word  r_PF_CL_DTDEchoSmoothPower[PF_CL_CHNLNUM],x						// Echopower for DTD
	voc_word  r_PF_CL_DTDMicNoisePower[PF_CL_CHNLNUM],x							// Mic Noise for DTDs
	voc_word  r_PF_CL_DTDMicNoiseFloorDevCnt[PF_CL_CHNLNUM],x					// Mic Noise Floor Deviation count for PF DTD Control Logic
	voc_word  r_PF_CL_DTDDecision[PF_CL_CHNLNUM],x								// Decision Value for DTD
	voc_word  r_PF_CL_DTD2ndDecision[PF_CL_CHNLNUM],x							// 2nd Decision Value for DTD

	

	voc_short pInstantSNR_t,x
	voc_short pInstantSNR_f,x

	voc_word SumMicPower,x
	voc_word SumSpkPower,x
	voc_word SumErrPower,x

	voc_short PFCLDTDOut[PF_CL_CHNLNUM],x	
// Fix g_PF_pSmoothErrPowerMin1[SUBBANDFILTER_NUM];
// Fix g_PF_pSmoothErrPowerMin2[SUBBANDFILTER_NUM];
// Fix g_PF_SmoothErrPowerSub1;

	voc_word g_PF_pSmoothErrPowerMin1[SUBBANDFILTER_NUM],x
	voc_word g_PF_pSmoothErrPowerMin2[SUBBANDFILTER_NUM],x
	voc_word g_PF_SmoothErrPowerSub1,x
	voc_word Temp_gsz,x

    voc_short m_PF_NoiseGainLimitAdj,x
    voc_short m_PF_NoiseGainLimitAdj_reserve,x



	
	voc_short GLOBAL_NON_CLIP_AEC_SPK_HISTORYDATA[160],x						// Spk Smooth Power for PF Control Logic

	voc_word AEC_CONST_0x00007fff,x
	voc_word AEC_CONST_0xffff8000,x

//    voc_short PITCH_FILTER_SPEAKER_ADDR              ,154+160	  ,x
//	voc_short PITCH_FILTER_SPEAKER_HISTORY_ADDR		 ,200+40	  ,x
//    voc_short PITCH_FILTER_MIC_HISTORY_ADDR			 ,200+40      ,x
//
//	voc_word  PITCH_FILTER_R1_MAX           			    ,x
//	voc_word  PITCH_FILTER_R2_MAX 						    ,x
//	voc_word  PITCH_FILTER_R3_MAX 						    ,x
//
//	voc_word  PITCH_FILTER_R1_NORM           			    ,x
//	voc_word  PITCH_FILTER_R2_NORM					        ,x
//	voc_word  PITCH_FILTER_R3_NORM 					        ,x
//
//	voc_short PITCH_FILTER_R1_INDEX           			    ,x
//	voc_short PITCH_FILTER_R2_INDEX 					    ,x
//	voc_short PITCH_FILTER_R3_INDEX 					    ,x
//	
//
//	voc_short PITCH_FILTER_R_M     						    ,x
//	voc_short PITCH_FILTER_R_M_OLD						    ,x
//	voc_short PITCH_FILTER_R_B						        ,x
//	voc_short PITCH_FILTER_R_B_OLD						    ,x
//	voc_short  PITCH_FILTER_COUNT   						,x



	voc_word r_PF_pSmoothErrPowerResBuf[SUBBANDFILTER_NUM],x
	voc_word r_PF_pSmoothSpkPowerResBuf[SUBBANDFILTER_NUM],x
	voc_word r_PF_PrevResEchoErrPowerBuf[SUBBANDFILTER_NUM],x
	voc_short r_PF_PrevResEchoAttErrdBBuf[SUBBANDFILTER_NUM],x
	voc_short r_PF_PrevResEchoWorkErrdBBuf[SUBBANDFILTER_NUM],x
	voc_word r_PF_SmoothErrPowerSub1Res,x
	voc_word r_PF_SmoothSpkPowerSub1Res,x
	voc_short r_PF_SpkSubErrdBSub1Res,x

	voc_short  r_HF_ROM[16],x
	voc_short  r_Log2Lin_HF_ROM[32],x



#endif


/**************************************************************************************
 * Function:    Coolsand_AEC
 *
 * Description: Echo cancelation 
 *
 * Inputs:      no
 *
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       08/13/2010
 **************************************************************************************/

void Coolsand_AEC(void)
{

	VoC_lw16i_short(REG0,0,DEFAULT);	

	VoC_push16(RA,DEFAULT);
	//VoC_lw16i_short(REG2,20,IN_PARALLEL);

	VoC_sw16_d(REG0,g_counter_subframe);

	//AEC reset
	VoC_jal(Coolsand_AECReset);


	VoC_lw16_d(REG2,DEC_OUTPUT_ADDR_ADDR);

	VoC_lw16i(REG0,OUTPUT_SPEECH_BUFFER2_ADDR);

	VoC_lw32_d(REG45,GLOBAL_RX_ADDR);

	VoC_be16_rr(Coolsand_AEC_Begin,REG0,REG2)
		VoC_lw16i(ACC0_HI,320);
		VoC_shr32_ri(ACC0,16,DEFAULT);
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);
Coolsand_AEC_Begin:
	VoC_and16_ri(REG5,0xBFFF);

	VoC_shr16_ri(REG2,1,DEFAULT);
    VoC_lw16i(REG3,80);
	VoC_jal(Rda_RunDma);

// VoC_directive: PARSER_OFF
	VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,1);
	VoC_NOP();
	VoC_NOP();
	fwrite(&RAM_X[((REGS[3].reg==OUTPUT_SPEECH_BUFFER1_ADDR) ? OUTPUT_SPEECH_BUFFER1_ADDR : OUTPUT_SPEECH_BUFFER2_ADDR)/2],1,320,AecSpkFileHandle);
	fflush(AecSpkFileHandle);
// VoC_directive: PARSER_ON

	// test
	//VoC_lw16d_set_inc(REG0, DEC_OUTPUT_ADDR_ADDR, 1);
	//VoC_lw32z(ACC0, DEFAULT);
	//VoC_lw32z(ACC1, IN_PARALLEL);
	//VoC_lw16i_short(ACC0_LO, 0, DEFAULT);
	//VoC_lw16i_short(ACC1_LO, 1, IN_PARALLEL);

	//VoC_loop_i(0, 160)
	//	VoC_sw16inc_p(ACC0_LO, REG0, DEFAULT);
	//	VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
	//	VoC_NOP();
	//VoC_endloop0

	// delay spk buffer
	// REG0 original buffer
	// REG1 delay buffer
	// REG4 num in short
	VoC_lw16_d(REG4, m_AECProcPara_DelaySampleNum);
	VoC_bez16_r(Coolsand_AEC_NOT_DELAY_SPK, REG4)
		VoC_lw16d_set_inc(REG0, DEC_OUTPUT_ADDR_ADDR, 1);
		VoC_lw16i_set_inc(REG1, GLOBAL_DEC_OUTPUT_DELAY_BUFFER2_ADDR, 1);

		VoC_lw16i(REG5, 160);
		VoC_lw16i(REG7, 1);

		// copy the last delay samples(REG2) to DELAY_BUFFER2(REG1)
		VoC_sub16_rr(REG6, REG5, REG4, DEFAULT);
		VoC_add16_rr(REG2, REG0, REG6, DEFAULT);

		VoC_lw16i_short(INC2, 1, DEFAULT);
		VoC_loop_r(0, REG4)
			VoC_lw16inc_p(ACC0_LO, REG2, DEFAULT);
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG1, DEFAULT);
		VoC_endloop0
		
		// delay the output samples(REG1 -> REG2)
		VoC_add16_rr(REG1, REG0, REG6, DEFAULT);

		VoC_sub16_rr(REG2, REG2, REG7, DEFAULT);
		VoC_sub16_rr(REG1, REG1, REG7, IN_PARALLEL);

		VoC_lw16i_short(INC2, -1, DEFAULT);
		VoC_lw16i_short(INC1, -1, IN_PARALLEL);
		VoC_loop_r(0, REG6)
			VoC_lw16inc_p(ACC0_LO, REG1, DEFAULT);
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG2, DEFAULT);
		VoC_endloop0

		// copy last delay buffer(REG1) to output buffer(REG0)
		VoC_lw16i_set_inc(REG1, GLOBAL_DEC_OUTPUT_DELAY_BUFFER1_ADDR, 1);
		VoC_lw16i_short(INC1, 1, DEFAULT);
		VoC_loop_r(0, REG4)
			VoC_lw16inc_p(ACC0_LO, REG1, DEFAULT);
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG0, DEFAULT);
		VoC_endloop0

		// copy delay buffer2 to delay buffer1
		VoC_lw16i_set_inc(REG1, GLOBAL_DEC_OUTPUT_DELAY_BUFFER1_ADDR, 1);
		VoC_lw16i_set_inc(REG2, GLOBAL_DEC_OUTPUT_DELAY_BUFFER2_ADDR, 1);
		VoC_loop_r(0, REG4)
			VoC_lw16inc_p(ACC0_LO, REG2, DEFAULT);
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG1, DEFAULT);
		VoC_endloop0
		// delay spk buffer end
Coolsand_AEC_NOT_DELAY_SPK:

// VoC_directive: PARSER_OFF
//	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);
//	VoC_NOP();
//	VoC_NOP();
//	fwrite(&RAM_X[((REGS[3].reg==INPUT_SPEECH_BUFFER1_ADDR) ? INPUT_SPEECH_BUFFER1_ADDR : INPUT_SPEECH_BUFFER2_ADDR)/2],1,320,TestFileHandle);
// 	fflush(TestFileHandle);
// VoC_directive: PARSER_ON

// VoC_directive: PARSER_OFF
	VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,1);
	VoC_NOP();
	VoC_NOP();
	fwrite(&RAM_X[((REGS[3].reg==OUTPUT_SPEECH_BUFFER1_ADDR) ? OUTPUT_SPEECH_BUFFER1_ADDR : OUTPUT_SPEECH_BUFFER2_ADDR)/2],1,320,AecDelaySpkFileHandle);
	fflush(AecDelaySpkFileHandle);
// VoC_directive: PARSER_ON



	VoC_lw16i(REG3,0x10);
	VoC_lw16i(REG4,0x7fff);
	VoC_lw16i_short(REG5,16,DEFAULT);
	VoC_bez16_d(Coolsand_AEC_L1,m_AECProcPara_StrongEchoFlag)

	VoC_lw16_d(REG3,GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR);
	VoC_lw16_d(REG4,GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR);
	VoC_lw16_d(REG5,GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR);

Coolsand_AEC_L1:
	VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,2);
	VoC_lw16i_set_inc(REG1,GLOBAL_NON_CLIP_TEMP_BUFFER,2);
	VoC_lw16i_set_inc(REG2,GLOBAL_NON_CLIP_AEC_SPK_HISTORYDATA,2);		
	VoC_sw16_d(REG3,GLOBAL_DIGITAL_GAIN_ADDR);
	VoC_sw16_d(REG4,GLOBAL_DIGITAL_MAXVALUE_ADDR);
	VoC_sw16_d(REG5,GLOBAL_NOISE_DIGITAL_GAIN_ADDR);

	VoC_lw16i(REG3,GLOBAL_AEC_DIGITAL_OLD_GAIN_ADDR);

	//VoC_jal(Coolsand_NonClipDigitalGain);

Coolsand_AEC_L0:		

		VoC_lw16i_short(REG7,8,DEFAULT);
		VoC_mult16_rd(REG7,REG7,g_counter_subframe);
		
		VoC_lw16_d(REG0,DEC_OUTPUT_ADDR_ADDR);
		VoC_lw16_d(REG1,ENC_INPUT_ADDR_ADDR);
		VoC_lw16_d(REG2,ENC_INPUT_ADDR_ADDR);
		
		VoC_add16_rr(REG0,REG0,REG7,DEFAULT);		
		VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

		VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

		//REG0:pSpk
		//REG1:pMic
		//REG2:IntBuf
		VoC_jal(Coolsand_EchoCancelProcessing);
		
		VoC_lw16i_short(REG1,1,DEFAULT);
		VoC_add16_rd(REG1,REG1,g_counter_subframe)
		VoC_lw16i_short(REG2,20,DEFAULT);
		VoC_sw16_d(REG1,g_counter_subframe);
		
		VoC_lw16_d(REG5,g_counter_subframe);
		
	VoC_bgt16_rr(Coolsand_AEC_L0,REG2,REG5)
		
		VoC_pop16(RA,DEFAULT);
		VoC_NOP();
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
		VoC_lw16i(ACC1_LO,0xffff);
		VoC_lw16i(ACC1_HI,0x7fff);
		VoC_lw16i_set_inc(REG0,g_PF_pSmoothErrPowerMin1,2);
		VoC_lw16i_set_inc(REG1,g_PF_pSmoothErrPowerMin2,2);
		VoC_loop_i(0,SUBBANDFILTER_NUM)
			VoC_sw32inc_p(ACC1,REG0,DEFAULT);
			VoC_sw32inc_p(ACC1,REG1,DEFAULT);
		VoC_endloop0
		VoC_sw32_d(ACC0,g_PF_SmoothErrPowerSub1);
		VoC_lw16i_set_inc(REG0,r_SpkAnaFilterDelayLine,2);
		VoC_lw16i_set_inc(REG1,r_MicAnaFilterDelayLine,2);
		VoC_lw16i_set_inc(REG2,r_OutSynFilterDelayLine,2);

		VoC_loop_i(0,PROTOTYPE_FILTER_LEN/2)
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);
			VoC_sw32inc_p(ACC0,REG1,DEFAULT);
		VoC_endloop0

		VoC_loop_i(0,PROTOTYPE_FILTER_LEN)
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
			
	 
//		VoC_lw32z(RL6,DEFAULT);
  //      VoC_lw16i_short(REG0,1,DEFAULT);
  //     	VoC_sw32_d(RL6,CONST_32bit0_addr);		
   //     VoC_sw16_d(REG0,CONST_1_addr); 
		//aec const

		VoC_lw32z(ACC0,DEFAULT);
		VoC_lw32z(ACC1,IN_PARALLEL);
		VoC_lw16i(ACC1_LO,0x20);
		VoC_lw16i_set_inc(REG0,r_PF_SmoothErrPower,2);
		VoC_lw16i_set_inc(REG1,r_PF_NoisePowerEstBuf,2);
		VoC_lw16i_set_inc(REG2,r_PF_PrevSigPowerNBuf,2);
		VoC_lw16i_set_inc(REG3,r_PF_PrevSigPowerEBuf,2);

		VoC_loop_i(0,SUBBANDFILTER_NUM)
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);
			VoC_sw32inc_p(ACC1,REG1,DEFAULT);
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
		
		VoC_lw16i_set_inc(REG3,GLOBAL_NON_CLIP_AEC_SPK_HISTORYDATA,2);
		VoC_loop_i(0,80)		
				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
		VoC_endloop0;

		
		VoC_lw32_d(REG45,GLOBAL_AEC_CONSTX_ADDR);
        VoC_lw16i(REG2,AnaFilterCoef/2);
        VoC_lw16i(REG3,GLOBAL_AEC_CONSTX_SIZE);
		VoC_jal(Rda_RunDma);


		//pitch filter  
		
//		VoC_lw32z(ACC0,DEFAULT);
//
//
//		VoC_lw16i_set_inc(REG3,PITCH_FILTER_SPEAKER_ADDR,2);
//		VoC_loop_i(0,(154+160)/2)		
//				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
//		VoC_endloop0;
//		
//		VoC_lw16i_set_inc(REG3,PITCH_FILTER_SPEAKER_HISTORY_ADDR,2);
//		VoC_loop_i(0,(240)/2)		
//				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
//		VoC_endloop0;
//
//				VoC_lw16i_set_inc(REG3,PITCH_FILTER_MIC_HISTORY_ADDR,2);
//		VoC_loop_i(0,(240)/2)		
//				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
//		VoC_endloop0;
//
//
//		VoC_sw32_d(ACC0,PITCH_FILTER_R1_MAX);
//		VoC_sw32_d(ACC0,PITCH_FILTER_R2_MAX);
//		VoC_sw32_d(ACC0,PITCH_FILTER_R3_MAX);
//		
//		VoC_sw32_d(ACC0,PITCH_FILTER_R1_NORM);
//		VoC_sw32_d(ACC0,PITCH_FILTER_R2_NORM);
//		VoC_sw32_d(ACC0,PITCH_FILTER_R3_NORM);
//
//
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R1_INDEX);
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R2_INDEX);
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R3_INDEX);
//
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R_M);
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R_M_OLD);
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R_B);
//		VoC_sw16_d(ACC0_LO,PITCH_FILTER_R_B_OLD);
// 		VoC_sw16_d(ACC0_LO,PITCH_FILTER_COUNT);
				
		// clear delay buffer
        //m_PF_NoiseGainLimitAdj = Double2Fix(150/128.0f, 10, 7);
        VoC_lw16i(REG7,150);
		VoC_lw32z(ACC0, DEFAULT);
        VoC_sw16_d(REG7,m_PF_NoiseGainLimitAdj);
		VoC_lw16i_set_inc(REG3,GLOBAL_DEC_OUTPUT_DELAY_BUFFER1_ADDR,2);
		VoC_loop_i(0,50)
				VoC_sw32inc_p(ACC0,REG3,DEFAULT);
		VoC_endloop0;

		// init saturate const
		VoC_lw16i(ACC0_LO, 0x7fff);
		VoC_lw16i(ACC1_HI, 0x0);
		VoC_lw16i(ACC1_LO, 0x8000);
		VoC_lw16i(ACC1_HI, 0xffff);
		VoC_sw32_d(ACC0, AEC_CONST_0x00007fff);
		VoC_sw32_d(ACC1, AEC_CONST_0xffff8000);

		VoC_lw32z(ACC0, DEFAULT);

		//const S32 HF_ROM[] = {30,27,25,23,20,18,16,14,12,11,9,7,6,4,3,1};
		VoC_lw16i_set_inc(REG0,r_HF_ROM,1);
		VoC_lw16i(REG1,30);
		VoC_lw16i(REG2,27);
		VoC_lw16i(REG3,25);
		VoC_lw16i(REG4,23);
		VoC_lw16i(REG5,20);
		VoC_lw16i(REG6,18);
		VoC_lw16i(REG7,16);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		VoC_lw16i(REG1,14);
		VoC_lw16i(REG2,12);
		VoC_lw16i(REG3,11);
		VoC_lw16i(REG4,9);
		VoC_lw16i(REG5,7);
		VoC_lw16i(REG6,6);
		VoC_lw16i(REG7,4);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		VoC_lw16i(REG1,3);
		VoC_lw16i(REG2,1);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);

		//const int HF_ROM[] = {32767, 26028, 20675, 16422, 13045, 10362,  8230, 6538, 5193,
		//4125,3276,2602,2067, 1642, 1304,1036, 823, 653 , 519 ,412,327,260,206,164,130,103,82,65,51, 41, 32,26};
		VoC_lw16i_set_inc(REG0,r_Log2Lin_HF_ROM,1);
		VoC_lw16i(REG1,32767);
		VoC_lw16i(REG2,26028);
		VoC_lw16i(REG3,20675);
		VoC_lw16i(REG4,16422);
		VoC_lw16i(REG5,13045);
		VoC_lw16i(REG6,10362);
		VoC_lw16i(REG7,8230);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		VoC_lw16i(REG1,6538);
		VoC_lw16i(REG2,5193);
		VoC_lw16i(REG3,4125);
		VoC_lw16i(REG4,3276);
		VoC_lw16i(REG5,2602);
		VoC_lw16i(REG6,2067);
		VoC_lw16i(REG7,1642);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		//1304,1036, 823, 653 , 519 ,412,327,260,206,164,130,103,82,65,
		VoC_lw16i(REG1,1304);
		VoC_lw16i(REG2,1036);
		VoC_lw16i(REG3,823);
		VoC_lw16i(REG4,653);
		VoC_lw16i(REG5,519);
		VoC_lw16i(REG6,412);
		VoC_lw16i(REG7,327);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		VoC_lw16i(REG1,260);
		VoC_lw16i(REG2,206);
		VoC_lw16i(REG3,164);
		VoC_lw16i(REG4,130);
		VoC_lw16i(REG5,103);
		VoC_lw16i(REG6,82);
		VoC_lw16i(REG7,65);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG0,DEFAULT);
		//51, 41, 32,26};
		VoC_lw16i(REG1,51);
		VoC_lw16i(REG2,41);
		VoC_lw16i(REG3,32);
		VoC_lw16i(REG4,26);
		VoC_sw16inc_p(REG1,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG3,REG0,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);

		
		
		VoC_lw16i_set_inc(REG0,r_PF_pSmoothErrPowerResBuf,2);
		VoC_lw16i_set_inc(REG1,r_PF_pSmoothSpkPowerResBuf,2);
		VoC_lw16i_set_inc(REG2,r_PF_PrevResEchoErrPowerBuf,2);

		VoC_loop_i(0,SUBBANDFILTER_NUM)
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);
			VoC_sw32inc_p(ACC0,REG1,DEFAULT);
			VoC_sw32inc_p(ACC0,REG2,DEFAULT);
		VoC_endloop0


		VoC_lw16i_set_inc(REG0,r_PF_PrevResEchoAttErrdBBuf,1);
		VoC_lw16i_set_inc(REG1,r_PF_PrevResEchoWorkErrdBBuf,1);

		VoC_loop_i(0,SUBBANDFILTER_NUM)
			VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
			VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
		VoC_endloop0

		VoC_sw32_d(ACC0,r_PF_SmoothErrPowerSub1Res);
		VoC_sw32_d(ACC0,r_PF_SmoothSpkPowerSub1Res);
		VoC_sw16_d(ACC0_LO,r_PF_SpkSubErrdBSub1Res);

Coolsand_AnalysisProcessing_L1:

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_EchoCancelProcessing
 *
 * Description: Echo cancel processing
 *
 * Inputs:       
 *
 *   REG0:pSpk [In]
 *   REG1:pMic [In]
 *   REG2:Pout [Out]
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       08/05/2010
 * Version 2.0  Created by  Xuml       05/13/2011
 **************************************************************************************/

void Coolsand_EchoCancelProcessing(void)
{
	VoC_push16(RA,DEFAULT);

	VoC_push16(REG2,DEFAULT);
	VoC_push16(REG1,DEFAULT);

	VoC_lw16i(REG1,r_SpkAnaFilterDelayLine);
	VoC_lw16i(REG4,g_SpkAnaFilterOutBufI);
	VoC_lw16i(REG5,g_SpkAnaFilterOutBufQ);

	//REG0:pSpkData
	//REG1:r_SpkAnaFilterDelayLine
	//REG4:g_SpkAnaFilterOutBufI
	//REG5:g_SpkAnaFilterOutBufQ


	 VoC_jal(Coolsand_AnalysisProcessing);




	VoC_pop16(REG0,DEFAULT);
	
	VoC_lw16i(REG1,r_MicAnaFilterDelayLine);
	VoC_lw16i(REG4,g_MicAnaFilterOutBufI);
	VoC_lw16i(REG5,g_MicAnaFilterOutBufQ);


	//REG0:pMicData
	//REG1:r_MicAnaFilterDelayLine
	//REG4:g_MicAnaFilterOutBufI
	//REG5:g_MicAnaFilterOutBufQ

	VoC_jal(Coolsand_AnalysisProcessing);





	VoC_lw16i_set_inc(REG0,r_AF_SpkSigBufI,1);
	VoC_lw16i_set_inc(REG1,r_AF_SpkSigBufQ,1);
	VoC_lw16i_set_inc(REG2,r_AF_FilterBufI,1);
	VoC_lw16i_set_inc(REG3,r_AF_FilterBufQ,1);
	VoC_sw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
	VoC_sw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);

	VoC_lw16i_set_inc(REG0,g_AF_ErrDataBufI,1);
	VoC_lw16i_set_inc(REG1,g_AF_ErrDataBufQ,1);
	VoC_lw16i_set_inc(REG2,g_AF_FilterOutBufI,1);
	VoC_lw16i_set_inc(REG3,g_AF_FilterOutBufQ,1);
	VoC_sw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);
	VoC_sw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);

	VoC_lw16i_set_inc(REG0,g_MicAnaFilterOutBufI,1);
	VoC_lw16i_set_inc(REG1,g_MicAnaFilterOutBufQ,1);
	VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
	VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);
	VoC_sw32_d(REG01,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
	VoC_lw16i_set_inc(REG0,r_AF_OldSpkDataBufI,1);
	VoC_lw16i_set_inc(REG1,r_AF_OldSpkDataBufQ,1);
	VoC_sw32_d(REG23,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

	VoC_lw16i_set_inc(REG2,r_AF_SpkSigPower,1);
	VoC_sw32_d(REG01,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);	
	VoC_sw16_d(REG2,TEMP_r_AF_SpkSigPower_addr_addr);

	VoC_lw16i_short(REG0,0,DEFAULT);
	VoC_NOP();
	VoC_sw16_d(REG0,g_ch);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_REF_DEBUG
	{   
		static int j = 0;
		static short buffer[9];
		fprintf(LogFileHandle,"/********************AF_DEBUG:%d***********************/\n",j++);
		if (j == 2756)
			printf("bug!\n");

		memcpy(buffer, &RAM_X[(g_MicAnaFilterOutBufI-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "Mic InputI: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);
		memcpy(buffer, &RAM_X[(g_MicAnaFilterOutBufQ-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "Mic InputQ: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);

		memcpy(buffer, &RAM_X[(g_SpkAnaFilterOutBufI-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "Spk InputI: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);
		memcpy(buffer, &RAM_X[(g_SpkAnaFilterOutBufQ-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "Spk InputQ: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);
	}
#endif
	// VoC_directive: PARSER_ON
		 
Coolsand_EchoCancelProcessing_L0:


	    // Filter
				
		VoC_jal(Coolsand_AdaptiveFilteringAmp);


		// Update Spk Power

		VoC_jal(Coolsand_UpdateSpkPower);

		// Filter Control
		
		VoC_lw16_d(REG7,g_ch);
		VoC_lw16i(REG2,g_SpkAnaFilterOutBufI);
		VoC_lw16i(REG3,g_SpkAnaFilterOutBufQ);
		VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
		VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

		VoC_lw16_d(REG6,m_AF_FrameCnt);

		VoC_lw16_p(REG2,REG2,DEFAULT);
		VoC_lw16_p(REG3,REG3,DEFAULT);


		//  REG2:  SubSpkDataI
		//  REG3:  SubSpkDataQ
		//  REG6: m_AF_FrameCnt  
		//  REG7: ch	


		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG_FIXSTEP
		{
			short *p = (short *)&RAM_X[(g_AF_StepSize-RAM_X_BEGIN_ADDR)/2];
			int pp = 0;
			for (; pp < 9; pp++)
				p[pp] = 127;
		}
	#endif
		// VoC_directive: PARSER_ON

		VoC_jal(Coolsand_FilterUpdateCotrol_Int);

		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
			short *p = (short *)&RAM_X[(g_AF_StepSize-RAM_X_BEGIN_ADDR)/2];
			short ch = *((short *)&RAM_X[(g_ch-RAM_X_BEGIN_ADDR)/2]+1);
			fprintf(LogFileHandle, "g_AF_StepSize[%d]: %d\n", ch, p[ch]);
		}
	#endif
		// VoC_directive: PARSER_ON

		// Update Filter

		VoC_lw16_d(REG7,g_ch);
		
		VoC_jal(Coolsand_UpdateFilterAmp);

		VoC_lw16i_short(REG1,1,DEFAULT);
		VoC_add16_rd(REG1,REG1,g_ch);
		VoC_lw16i_short(REG2,9,DEFAULT);
		VoC_sw16_d(REG1,g_ch);
		
		VoC_bgt16_rr(Coolsand_EchoCancelProcessing_L0,REG2,REG1)
			
//tempFixQ
//#ifdef DUMP_AF_ERR_OUT_DATA
#if 0
	VoC_lw16i_set_inc(REG0,g_AF_ErrDataBufI,1);
	VoC_lw16i_set_inc(REG1,g_AF_ErrDataBufQ,1);

	VoC_lw16i_set_inc(REG2,g_OutSynFilterInBufI,1);
	VoC_lw16i_set_inc(REG3,g_OutSynFilterInBufQ,1);

       VoC_loop_i(0, 9)
		VoC_lw16inc_p(REG4,REG0,DEFAULT);
		VoC_lw16inc_p(REG5,REG1,DEFAULT);
		VoC_sw16inc_p(REG4,REG2,DEFAULT);
		VoC_sw16inc_p(REG5,REG3,DEFAULT);
        VoC_endloop0
#endif
// VoC_directive: PARSER_ON


//#ifdef POST_FILTER_NS		
#if 1
			//g_DTD_ch
			//CLChnl

			VoC_lw16i_short(REG0,0*2,DEFAULT);
			VoC_lw16i_short(REG1,1,DEFAULT);

			VoC_sw16_d(REG0,g_DTD_ch);
			VoC_sw16_d(REG1,CLChnl);
			
			VoC_lw16i_set_inc(REG2,g_MicAnaFilterOutBufI,1);
			VoC_lw16i_set_inc(REG3,g_MicAnaFilterOutBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);
			{
				VoC_NOP();
				VoC_NOP();
				//short *pMicAnaFilterOutBufI = &RAM_X[REGS[2].reg/2];
				//short *pMicAnaFilterOutBufQ = &RAM_X[REGS[3].reg/2];
			}				
			VoC_jal(Coolsand_SumInstantPower);

			VoC_movreg32(RL6,ACC0,DEFAULT);
				
			VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
			VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);
			{
				VoC_NOP();
				VoC_NOP();
				//short *pSpkAnaFilterOutBufI = &RAM_X[REGS[2].reg/2];
				//short *pSpkAnaFilterOutBufQ = &RAM_X[REGS[3].reg/2];
			}
			VoC_jal(Coolsand_SumInstantPower);

			VoC_movreg32(RL7,ACC0,DEFAULT);
			
			VoC_lw16i_set_inc(REG2,g_AF_ErrDataBufI,1);
			VoC_lw16i_set_inc(REG3,g_AF_ErrDataBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);
			{
				VoC_NOP();
				VoC_NOP();
				//short *pAF_ErrDataBufI = &RAM_X[REGS[2].reg/2];
				//short *pAF_ErrDataBufQ = &RAM_X[REGS[3].reg/2];
			}
			VoC_jal(Coolsand_SumInstantPower);

			
			VoC_sw32_d(RL6,SumMicPower);
			VoC_sw32_d(RL7,SumSpkPower);
			VoC_sw32_d(ACC0,SumErrPower);

			VoC_jal(Coolsand_ControlLogicDTD);	

			VoC_sw16_d(REG0,PFCLDTDOut);

			VoC_lw16i_short(REG0,1*2,DEFAULT);
			VoC_lw16i_short(REG1,3,DEFAULT);

			VoC_sw16_d(REG0,g_DTD_ch);
			VoC_sw16_d(REG1,CLChnl);

			VoC_lw16i_set_inc(REG2,g_MicAnaFilterOutBufI,1);
			VoC_lw16i_set_inc(REG3,g_MicAnaFilterOutBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);
				
			VoC_jal(Coolsand_SumInstantPower);

			VoC_add32_rd(RL6,ACC0,SumMicPower);
			
			VoC_lw16i_set_inc(REG2,g_SpkAnaFilterOutBufI,1);
			VoC_lw16i_set_inc(REG3,g_SpkAnaFilterOutBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);

			VoC_jal(Coolsand_SumInstantPower);

			VoC_add32_rd(RL7,ACC0,SumSpkPower);
			
			VoC_lw16i_set_inc(REG2,g_AF_ErrDataBufI,1);
			VoC_lw16i_set_inc(REG3,g_AF_ErrDataBufQ,1);

			VoC_add16_rd(REG2,REG2,CLChnl);
			VoC_add16_rd(REG3,REG3,CLChnl);

			VoC_jal(Coolsand_SumInstantPower);

			VoC_add32_rd(ACC0,ACC0,SumErrPower);

			VoC_shr32_ri(RL6,1,DEFAULT);
			VoC_shr32_ri(RL7,1,IN_PARALLEL);

			VoC_shr32_ri(ACC0,1,DEFAULT);

			VoC_push32(RL6,DEFAULT);

			VoC_push32(RL7,DEFAULT);

			VoC_push32(ACC0,DEFAULT);

			VoC_jal(Coolsand_ControlLogicDTD);	
			
			VoC_sw16_d(REG0,PFCLDTDOut+1);

			VoC_lw16i_short(REG1,0,DEFAULT);
			
			VoC_bnez16_r(Coolsand_EchoCancelProcessing_L3,REG0)
			VoC_bltz32_d(Coolsand_EchoCancelProcessing_L3,r_PF_CL_DTDDecision)
				VoC_sw16_d(REG1,PFCLDTDOut);
Coolsand_EchoCancelProcessing_L3:

			VoC_pop32(ACC0,DEFAULT);
			VoC_pop32(RL7,DEFAULT);
			VoC_pop32(RL6,DEFAULT);
			
			
			//RL6:SumMicPower
			//RL7:SumSpkPower
			//ACC0:SumErrPower
			VoC_movreg32(REG45,RL7,DEFAULT);
			VoC_push32(ACC0,IN_PARALLEL);

			VoC_shr32_ri(REG45,-1,DEFAULT);
			VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

			VoC_lw16_d(REG7,m_AECProcPara_PFCLChanlGain);

			VoC_shru16_ri(REG4,1,DEFAULT);
			VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

			VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);

			VoC_macX_rr(REG5,REG7,DEFAULT);

			VoC_NOP();

			VoC_pop32(ACC0,DEFAULT);
			VoC_movreg32(RL7,ACC0,IN_PARALLEL);

			VoC_shr32_ri(RL7,-8,DEFAULT);

			VoC_shr32_ri(RL7,8,DEFAULT);
			
			VoC_jal(Coolsand_ControlLogicMergeChnlEnhancedDTD);

			VoC_jal(Coolsand_ControlLogicSingleChnlEnhanced);
			
			VoC_lw16i_short(REG0,0,DEFAULT);
			VoC_NOP();
			VoC_sw16_d(REG0,g_ch);
	exit_on_warnings = 1;
Coolsand_EchoCancelProcessing_L1:

		VoC_lw16_d(REG7,g_ch);
		//		if (ch == 0)
		VoC_bnez16_r(Coolsand_EchoCancelProcessing_L11,REG7)
					// Compute Instant Err Power
		//			InstantErrPowerSub1.f = g_AF_ErrDataBufI[1].f;
		//			InstantErrPowerSub1.t = g_AF_ErrDataBufI[1].f+1;
		VoC_lw16i_set_inc(REG0,(g_AF_ErrDataBufI+1),1);
		VoC_lw16i_set_inc(REG1,(g_AF_ErrDataBufQ+1),1);
//		VoC_lw16i_short(ACC0_HI,0,DEFAULT);
		VoC_lw16_p(ACC0_HI,REG0,DEFAULT);
		VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);		//			Truncate(InstantErrPowerSub1, 32, 31);
		VoC_lw16_p(ACC1_HI,REG1,DEFAULT);
		VoC_NOP();
		//VoC_NOP();
		//VoC_NOP();
		VoC_shr32_ri(ACC0,16,DEFAULT);
		VoC_shr32_ri(ACC1,16,IN_PARALLEL);
		//			CmplxAppxNorm(&g_AF_ErrDataBufI[1], &g_AF_ErrDataBufQ[1], &InstantErrPowerSub1);
		VoC_jal(Coolsand_CmplxAppxNorm_int);	// return ACC0
        VoC_bngt32_rd(Coolsand_EchoCancelProcessing_not_saturation_L00,ACC0,AEC_CONST_0x00007fff)
            VoC_lw32_d(ACC0,AEC_CONST_0x00007fff);
Coolsand_EchoCancelProcessing_not_saturation_L00:
        VoC_bgt32_rd(Coolsand_EchoCancelProcessing_not_saturation_L01,ACC0,AEC_CONST_0xffff8000)
            VoC_lw32_d(ACC0,AEC_CONST_0xffff8000);
Coolsand_EchoCancelProcessing_not_saturation_L01:
		//InstantErrPowerSub1 = InstantErrPowerSub1*InstantErrPowerSub1;
		VoC_NOP();
		VoC_movreg32(REG45,ACC0,DEFAULT);
		VoC_multf32_rr(RL7,REG4,REG4,DEFAULT);
		//			if (InstantErrPowerSub1 > g_PF_SmoothErrPowerSub1)
		//				ErrSmoothFactor = Double2Fix(1.0/4, 5, 4);
		//			else
		//				ErrSmoothFactor = Double2Fix(1.0/16, 5, 4);
		VoC_lw16i_short(REG1,4,DEFAULT);
//		VoC_shr32_ri(RL7,-4,DEFAULT);
		VoC_bgt32_rd(Coolsand_EchoCancelProcessing_L12,RL7,g_PF_SmoothErrPowerSub1)
			VoC_lw16i_short(REG1,1,DEFAULT);
Coolsand_EchoCancelProcessing_L12:
		//			g_PF_SmoothErrPowerSub1 = FirstOrderIIRSmooth2(&g_PF_SmoothErrPowerSub1, &InstantErrPowerSub1, &ErrSmoothFactor); 
		// extern VoC_jal(Coolsand_FirstOrderIIRSmooth2);
	//Truncate(NewValue, pOldValue->f-pUpdateFactor->f+1, pOldValue->f-pUpdateFactor->f);
		VoC_lw16i_short(FORMAT32,4,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rd(REG45,RL7,g_PF_SmoothErrPowerSub1);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG4,REG1,DEFAULT);
		VoC_lw16i_short(FORMAT32,-11,DEFAULT);
		VoC_multf32_rr(REG45,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_add32_rd(RL7,RL7,g_PF_SmoothErrPowerSub1);	//	NewValue = NewValue+(*pOldValue);
		VoC_add32_rr(REG45,REG45,RL7,DEFAULT);
		VoC_NOP();
		VoC_sw32_d(REG45,g_PF_SmoothErrPowerSub1);
Coolsand_EchoCancelProcessing_L11:
		VoC_jal(Coolsand_ComputeNoiseWeight_DeciDirectCTRL);
	
		VoC_jal(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL);
#if 1

		VoC_lw16_d(REG0,m_AECProcPara_ResidualEchoEnable);
		VoC_lw16_d(REG7,g_ch);
		VoC_bez16_r(Coolsand_EchoCancelProcessing_L13,REG0);
		
		//		if (ch == 0)
		VoC_bnez16_r(Coolsand_EchoCancelProcessing_L13,REG7);
		
		//TempErrDataI = (S16)g_AF_ErrDataBufI[1];
		//TempErrDataQ = (S16)g_AF_ErrDataBufQ[1];
		VoC_lw16i(REG0,(g_AF_ErrDataBufI+1));
		VoC_lw16i(REG1,(g_AF_ErrDataBufQ+1));
		VoC_lw16_p(REG0,REG0,DEFAULT);
		VoC_lw16_p(REG1,REG1,DEFAULT);
		VoC_NOP();
		VoC_jal(Coolsand_CmplxAppxNorm);

	//FixLimit()
	VoC_lw32_d(RL6, AEC_CONST_0x00007fff); //MAX_1
	VoC_lw32_d(ACC1, AEC_CONST_0xffff8000); //MAX
	VoC_bngt32_rr(ECHO_FIX_L1,ACC0,RL6);
		VoC_movreg32(ACC0,RL6,DEFAULT)
		VoC_jump(ECHO_FIX_L2);
ECHO_FIX_L1:
	VoC_bngt32_rr(ECHO_FIX_L2,ACC1,ACC0);

		VoC_movreg32(ACC0,ACC1,DEFAULT);
ECHO_FIX_L2:
		//InstantErrPowerSub1Res = InstantErrPowerSub1Res*InstantErrPowerSub1Res;
		//Truncate(InstantErrPowerSub1Res, 32, 31);
		VoC_lw16i_short(FORMAT32,-1,DEFAULT);
		VoC_movreg32(REG45,ACC0,DEFAULT);
		VoC_lw16i_short(REG1,4,DEFAULT);
		VoC_lw32_d(ACC0,r_PF_SmoothErrPowerSub1Res);
		VoC_multf32_rr(RL7,REG4,REG4,DEFAULT);
		VoC_NOP();
			
		VoC_bgt32_rr(Coolsand_EchoCancelProcessing_L14,RL7,ACC0)
		
		VoC_lw16i_short(REG1,1,DEFAULT);
Coolsand_EchoCancelProcessing_L14:

		VoC_lw16i_short(FORMAT32,-11,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_NOP();
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_lw16i_short(FORMAT32,4,DEFAULT);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_NOP();
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
		VoC_NOP();
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT); //	NewValue = NewValue+(*pOldValue);
		VoC_NOP();
		VoC_sw32_d(REG45,r_PF_SmoothErrPowerSub1Res);
		
		
		//Lin2Log(&r_PF_SmoothErrPowerSub1Res, &InstantErrPowerdBSub1);
		//input: REG01, Q31, output: REG0
		VoC_movreg32(REG01,REG45,DEFAULT);
		VoC_jal(Coolsand_Lin2Log);
		VoC_push16(REG0,DEFAULT);
				
		//TempSpkSigBufISub1 = r_AF_SpkSigBufI[1][0];
		//TempSpkSigBufQSub1 = r_AF_SpkSigBufQ[1][0];
		VoC_lw16i(REG0,(r_AF_SpkSigBufI+8));
		VoC_lw16i(REG1,(r_AF_SpkSigBufQ+8));
		VoC_lw16_p(REG0,REG0,DEFAULT);
		VoC_lw16_p(REG1,REG1,DEFAULT);
		
		VoC_jal(Coolsand_CmplxAppxNorm);
		
		//InstantSpkPowerSub1Res = InstantSpkPowerSub1Res*InstantSpkPowerSub1Res;
		//Truncate(InstantSpkPowerSub1Res, 32, 31);
		
		VoC_lw16i_short(FORMAT32,-1,DEFAULT);
		VoC_movreg32(REG45,ACC0,DEFAULT);
		VoC_lw16i_short(REG1,4,DEFAULT);
		VoC_lw32_d(ACC0,r_PF_SmoothSpkPowerSub1Res);
		VoC_multf32_rr(RL7,REG4,REG4,DEFAULT);
		VoC_NOP();
		
		VoC_bgt32_rr(Coolsand_EchoCancelProcessing_L15,RL7,ACC0)
		
		VoC_lw16i_short(REG1,1,DEFAULT);
Coolsand_EchoCancelProcessing_L15:

		VoC_lw16i_short(FORMAT32,0,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_NOP();
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_NOP();
		VoC_shr32_ri(RL7,-11,DEFAULT);
		VoC_shr32_ri(RL6,4,DEFAULT);
		//VoC_NOP();
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
		VoC_NOP();
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT); //	NewValue = NewValue+(*pOldValue);
		VoC_NOP();
		VoC_sw32_d(REG45,r_PF_SmoothSpkPowerSub1Res);
		
		//Lin2Log(&r_PF_SmoothErrPowerSub1Res, &InstantErrPowerdBSub1);
		//input: REG01, Q31, output: REG0
		VoC_movreg32(REG01,REG45,DEFAULT);
		VoC_jal(Coolsand_Lin2Log);
					
		VoC_pop16(REG1,DEFAULT);
		VoC_sub16_rr(REG3,REG0,REG1,DEFAULT);
		VoC_NOP();
		VoC_sw16_d(REG3,r_PF_SpkSubErrdBSub1Res);
		
Coolsand_EchoCancelProcessing_L13:
				
#endif

		VoC_jal(Coolsand_PostFiltering);

		VoC_lw16i_short(REG1,1,DEFAULT);
		VoC_add16_rd(REG1,REG1,g_ch);
		VoC_lw16i_short(REG2,9,DEFAULT);
		VoC_sw16_d(REG1,g_ch);
		
	VoC_bgt16_rr(Coolsand_EchoCancelProcessing_L1,REG2,REG1)
		//
		// Scale DoubleTalk Output
		//

		VoC_lw16i_short(REG0,20,DEFAULT);
		VoC_lw16_d(REG1,r_PF_CL_State);
		VoC_lw16_d(REG2,m_AECProcPara_StrongEchoFlag);	

	/*	VoC_bez16_r(Coolsand_EchoCancelProcessing_L2,REG2)
		VoC_bne16_rr(Coolsand_EchoCancelProcessing_L2,REG0,REG1)

			VoC_lw16i(REG4,g_AF_ErrDataBufI);
			VoC_lw16i(REG5,g_AF_ErrDataBufQ);

			VoC_lw16i(REG6,g_OutSynFilterInBufI);
			VoC_lw16i(REG7,g_OutSynFilterInBufQ);

			VoC_lw32z(RL6,DEFAULT);
			VoC_lw32z(RL7,IN_PARALLEL);
			
			VoC_push32(REG67,DEFAULT);//g_OutSynFilterInBufI and g_OutSynFilterInBufQ
			VoC_lw16i_short(REG3,0,IN_PARALLEL);

			VoC_loop_i_short(2,DEFAULT)			
			VoC_startloop0

				VoC_lw16_p(REG0,REG4,DEFAULT);
				VoC_add16_rr(REG6,REG6,REG3,IN_PARALLEL);

				VoC_lw16_p(REG1,REG5,DEFAULT);				
				VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

				VoC_jal(Coolsand_CmplxAppxNorm);

				VoC_add32_rr(RL7,RL7,ACC0,DEFAULT);

				VoC_shr32_ri(RL7,-16,DEFAULT);
				VoC_lw16_p(REG0,REG6,IN_PARALLEL);

				VoC_shr32_ri(RL7,16,DEFAULT);
				VoC_lw16_p(REG1,REG7,IN_PARALLEL);

				VoC_jal(Coolsand_CmplxAppxNorm);
				
				VoC_add32_rr(RL6,RL6,ACC0,DEFAULT);
				VoC_lw16i_short(REG3,1,IN_PARALLEL);

				VoC_shr32_ri(RL6,-16,DEFAULT);
				VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);

				VoC_shr32_ri(RL6,16,DEFAULT);
				VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);
				
			VoC_endloop0

			VoC_lw16i_short(REG2,16,DEFAULT);

			VoC_jal(Coolsand_MSBPos);

			VoC_lw16i_short(REG7,8,DEFAULT);
			VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);

			VoC_sub16_rr(REG7,REG3,REG7,DEFAULT);
			VoC_lw16i_short(REG5,3,IN_PARALLEL);

			VoC_shr16_rr(REG6,REG7,DEFAULT);
			VoC_lw16i_short(FORMAT16,2-16,IN_PARALLEL);

			VoC_multf16_rr(REG6,REG6,REG5,DEFAULT);

			VoC_pop32(REG01,DEFAULT);//g_OutSynFilterInBufI and g_OutSynFilterInBufQ

			VoC_shr16_ri(REG6,-1,DEFAULT);
			VoC_lw16i_short(INC0,1,IN_PARALLEL);

			VoC_shr16_ri(REG6,1,DEFAULT);
			VoC_lw16i_short(INC1,1,IN_PARALLEL);
		
			VoC_loop_i_short(2,DEFAULT)
			VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);
			VoC_startloop0

				VoC_multf16_rp(REG2,REG6,REG0,DEFAULT);
				
				VoC_multf16_rp(REG3,REG6,REG1,DEFAULT);

				VoC_NOP();

				VoC_sw16inc_p(REG2,REG0,DEFAULT);

				VoC_sw16inc_p(REG3,REG1,DEFAULT);
				
			VoC_endloop0

Coolsand_EchoCancelProcessing_L2:
*/
#endif

	// Conj to Symmetry Channel

	VoC_lw16i_set_inc(REG2,g_OutSynFilterInBufI+SUBBAND_CHANNEL_NUM-SUBBANDFILTER_NUM,-1);
	VoC_lw16i_set_inc(REG3,g_OutSynFilterInBufQ+SUBBAND_CHANNEL_NUM-SUBBANDFILTER_NUM,-1);

	VoC_lw16i_set_inc(REG0,g_OutSynFilterInBufI+SUBBANDFILTER_NUM,1);
	VoC_lw16i_set_inc(REG1,g_OutSynFilterInBufQ+SUBBANDFILTER_NUM,1);
	
			
	VoC_loop_i_short(7,DEFAULT)
	VoC_lw16i_short(REG6,0,IN_PARALLEL);
	VoC_startloop0

		VoC_lw16inc_p(REG4,REG2,DEFAULT);
		VoC_sub16inc_rp(REG5,REG6,REG3,DEFAULT);

		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG1,DEFAULT);			
		
	VoC_endloop0

	VoC_pop16(REG3,DEFAULT);

	VoC_lw16i(REG0,g_OutSynFilterInBufI);
	VoC_lw16i(REG1,g_OutSynFilterInBufQ);
	VoC_lw16i(REG2,r_OutSynFilterDelayLine);

	/*
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	{
		short *ptr = (short *)&RAM_X[(r_OutSynFilterDelayLine-RAM_X_BEGIN_ADDR)/2];
		int buffer[128];
		int i = 0;
		for (i = 0; i < 128; i++)
			buffer[i] = ptr[i];
		fwrite(buffer, 4, 128, OutSynFilterDelayLine_fileid);
	}
#endif
// VoC_directive: PARSER_ON
	*/

#ifdef AEC_VOC_SYN_DEBUG
	VoC_push16(REG3, DEFAULT);
#endif

	//REG0:g_OutSynFilterInBufI
	//REG1:g_OutSynFilterInBufQ
	//REG2:r_OutSynFilterDelayLine
	//REG3:pOutData
	VoC_jal(Coolsand_SynthesisProcessing);

#ifdef AEC_VOC_SYN_DEBUG
	VoC_pop16(REG3, DEFAULT);
#endif
	
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	{
		int *ptr = (int *)&RAM_X[(r_OutSynFilterDelayLine-RAM_X_BEGIN_ADDR)/2];
		int buffer[128];
		int i = 0;
		for (i = 0; i < 128; i++)
			buffer[i] = ptr[i];
		fwrite(buffer, 4, 128, OutSynFilterDelayLine_fileid);
	}
#endif
// VoC_directive: PARSER_ON

	VoC_lw16i_short(REG1,1,DEFAULT);
	VoC_add16_rd(REG1,REG1,m_AF_FrameCnt);
	VoC_pop16(RA,DEFAULT);
	VoC_sw16_d(REG1,m_AF_FrameCnt);
	VoC_return;	
}






/**************************************************************************************
 * Function:    Coolsand_AnalysisProcessing
 *
 * Description: Analysis Filter Bank Operation
 *
 * Inputs:       
 *
 *   REG0:(pInData) pointer to Input Data [In]
 *   REG1:(pTapppedDelayLine): DelayLine [In/Out]
 *	
 *   REG4:(pOutDataI)pointer to Output Data [Out]
 *   REG5:(pOutDataQ)pointer to Output Data [Out]   
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/

void Coolsand_AnalysisProcessing(void)
{

// VoC_directive: PARSER_OFF
   // printf("AnalysisProcessing......\n");
// VoC_directive: PARSER_ON
	
	VoC_push16(RA,DEFAULT);	
	//pTapppedDelayLine+SUBSAMPLING_RATE
	VoC_movreg16(REG3,REG1,DEFAULT);
	VoC_lw16i_short(REG6,SUBSAMPLING_RATE,IN_PARALLEL);

	VoC_add16_rr(REG2,REG1,REG6,DEFAULT);
	VoC_lw16i_short(REG7,PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE,IN_PARALLEL);



	//REG1: pTapppedDelayLine;
	//REG2: pTapppedDelayLine+SUBSAMPLING_RATE;
	//REG7: PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;

	VoC_jal(Coolsand_BackCopyVec);

	VoC_lw16i_short(REG7,SUBSAMPLING_RATE,DEFAULT);

	VoC_movreg16(REG1,REG0,DEFAULT);
	VoC_lw16i_short(INC1,1,IN_PARALLEL);
		
	VoC_movreg16(REG2,REG3,DEFAULT);
	VoC_lw16i_short(INC2,-1,IN_PARALLEL);
	
	//REG1: pInData;
	//REG2: pTapppedDelayLine;
	//REG7: SUBSAMPLING_RATE;

	VoC_jal(Coolsand_FlipVec);

	VoC_movreg16(REG1,REG3,DEFAULT);
	VoC_lw16i_short(FORMAT16,-3,IN_PARALLEL);

	VoC_lw16i(REG2,AnaFilterCoef);
	VoC_lw16i(REG3,g_FilterBank_MulBuf);
	VoC_lw16i(REG0,PROTOTYPE_FILTER_LEN);

	//REG1: pTapppedDelayLine;
	//REG2: AnaFilterCoef;
	//REG3: g_FilterBank_MulBuf;
	//REG0: SUBSAMPLING_RATE;
	VoC_jal(Coolsand_MulVec);

	
	VoC_movreg16(REG1,REG4,DEFAULT);
	VoC_lw16i(REG7,SUBBAND_CHANNEL_NUM);
	
	//REG1: g_FilterBank_FoldSumBuf;
	//REG7: SUBBAND_CHANNEL_NUM;
	VoC_jal(Coolsand_ClearVec);

	
	VoC_lw16i_short(INC0,1,DEFAULT);
	VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

	VoC_lw16i(REG1,g_FilterBank_MulBuf);

	VoC_lw16i_short(INC1,1,DEFAULT);
	VoC_lw16i_short(INC2,1,IN_PARALLEL);	

	VoC_loop_i(1,PROTOTYPE_FILTER_LEN/SUBBAND_CHANNEL_NUM)
		
		VoC_movreg16(REG0,REG4,DEFAULT);
		VoC_movreg16(REG2,REG4,IN_PARALLEL);

		//REG0: pOutDataI;
		//REG1: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM;
		//REG2: pOutDataI;
		//REG7: SUBBAND_CHANNEL_NUM;

		VoC_jal(Coolsand_AddVec);

	VoC_endloop1


	VoC_movreg16(REG1,REG5,DEFAULT);
	VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

	//REG1: pOutDataQ;
	//REG7: SUBBAND_CHANNEL_NUM;
	VoC_jal(Coolsand_ClearVec);


	VoC_movreg16(REG1,REG5,DEFAULT);
	VoC_movreg16(REG0,REG4,IN_PARALLEL);

	VoC_push32(REG45,DEFAULT);
	
		
	//REG0:pOutDataI;
	//REG1:pOutDataQ;


	VoC_jal(Coolsand_IFFT16);


	//ScaleVec
	VoC_pop32(REG01,DEFAULT);

	VoC_lw16i_short(INC0,2,DEFAULT);
	VoC_lw16i_short(INC1,2,IN_PARALLEL);

	VoC_loop_i_short(8,DEFAULT);
	VoC_startloop0

		VoC_lw32_p(REG45,REG0,DEFAULT);
		VoC_lw32_p(REG67,REG1,DEFAULT);

		VoC_shr16_ri(REG4,-1,DEFAULT);
		VoC_shr16_ri(REG5,-1,IN_PARALLEL);

		VoC_shr16_ri(REG6,-1,DEFAULT);
		VoC_shr16_ri(REG7,-1,IN_PARALLEL);

		VoC_sw32inc_p(REG45,REG0,DEFAULT);
		VoC_sw32inc_p(REG67,REG1,DEFAULT);

	VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_SynthesisProcessing
 *
 * Description: Analysis Filter Bank Operation
 *
 * Inputs:       
 *
 *   REG0: pInDataI: pointer to Input IData [In]
 *   REG1: pInDataQ: pointer to Input QData [In]
 *
 *   REG2: pTapppedDelayLine: DelayLine [In/Out]
 *   REG3: pOutData[out]
 *
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       07/28/2010
 **************************************************************************************/

void Coolsand_SynthesisProcessing(void)
{


// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	{
		short buffer[16] = {0};
		memcpy(buffer, &RAM_X[(g_OutSynFilterInBufI-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "SynthesisProcessing InputI: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);
		memcpy(buffer, &RAM_X[(g_OutSynFilterInBufQ-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "SynthesisProcessing InputQ: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7], buffer[8]);
	}
#endif
// VoC_directive: PARSER_ON

	VoC_push16(RA,DEFAULT);	
	VoC_push32(REG23,IN_PARALLEL);

	VoC_push32(REG01,DEFAULT);
		
	//REG0:pInDataI
	//REG1:pInDataQ

	VoC_jal(Coolsand_IFFT16);

	VoC_pop32(REG01,DEFAULT);

	VoC_movreg32(RL7,REG01,DEFAULT);
	VoC_lw16i_short(INC0,2,IN_PARALLEL);

	VoC_movreg16(REG2,REG0,DEFAULT);
	VoC_lw16i_short(INC2,2,IN_PARALLEL);
	
	VoC_loop_i_short(8,DEFAULT);
	VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
	VoC_startloop0
		
		VoC_shr16_ri(REG4,-3,DEFAULT);
		VoC_shr16_ri(REG5,-3,IN_PARALLEL);

		VoC_lw32inc_p(REG45,REG0,DEFAULT);

		VoC_sw32inc_p(REG45,REG2,DEFAULT);
	VoC_endloop0

	VoC_movreg32(REG01,RL7,DEFAULT);
	VoC_lw16i_short(INC1,2,IN_PARALLEL);

	VoC_lw16i_set_inc(REG2,g_FilterBank_MulBuf,2)

	
	VoC_loop_i(1,PROTOTYPE_FILTER_LEN/SUBBAND_CHANNEL_NUM)

		VoC_movreg16(REG1,REG0,DEFAULT);
		VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM,IN_PARALLEL);

		//REG1: pInDataI;
		//REG2: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM;
		//REG7: SUBBAND_CHANNEL_NUM;
		VoC_jal(Coolsand_CopyVec);	
		
	VoC_endloop1

/*
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	{
		short *ptr = (short *)&RAM_X[(g_FilterBank_MulBuf-RAM_X_BEGIN_ADDR)/2];
		int buffer[128];
		int i = 0;
		for (i = 0; i < 128; i++)
			buffer[i] = ptr[i];
		fwrite(buffer, 4, 128, FilterBankMulBuf_fileid);
	}
#endif
// VoC_directive: PARSER_ON
*/

	VoC_lw16i(REG1,g_FilterBank_MulBuf);
	VoC_lw16i(REG2,SynFilterCoef);
	
	VoC_movreg16(REG3,REG1,DEFAULT);
	VoC_lw16i_short(FORMAT16,-3,IN_PARALLEL);

	VoC_lw16i(REG0,PROTOTYPE_FILTER_LEN);

	//REG1: g_FilterBank_MulBuf;
	//REG2: SynFilterCoef;
	//REG3: g_FilterBank_MulBuf;
	//REG0: SUBSAMPLING_RATE;
#ifndef AEC_VOC_SYN_DEBUG
	VoC_jal(Coolsand_MulVec);
#else
// VoC_directive: PARSER_OFF
	{
		int i = 0;
		short *pFilterBankMulBuf = (short *)&RAM_X[(g_FilterBank_MulBuf-RAM_X_BEGIN_ADDR)/2];
		short *pSynFilterCoef = (short *)&RAM_X[(SynFilterCoef-RAM_X_BEGIN_ADDR)/2];
		for (; i < PROTOTYPE_FILTER_LEN; i++)
		{
			int temp = pFilterBankMulBuf[i] * pSynFilterCoef[i];
			temp = temp >> 13;
			if (temp > 32767)
			{
				temp = 32767;
				OverflowPrint(1670, "Coolsand_SynthesisProcessing");
			}
			else if (temp < -32768)
			{
				temp = -32768;
				OverflowPrint(1675, "Coolsand_SynthesisProcessing");
			}

			pFilterBankMulBuf[i] = temp;
		}
	}
// VoC_directive: PARSER_ON
#endif


// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	{
		short *ptr = (short *)&RAM_X[(g_FilterBank_MulBuf-RAM_X_BEGIN_ADDR)/2];
		int buffer[128];
		int i = 0;
		for (i = 0; i < 128; i++)
			buffer[i] = ptr[i];
		fwrite(buffer, 4, 128, FilterBankMulBuf_fileid);
	}
#endif
// VoC_directive: PARSER_ON
	
	VoC_pop32(REG45,DEFAULT);//pTapppedDelayLine pOutData
	VoC_lw16i_short(INC2,2,IN_PARALLEL);

	VoC_lw16i_short(INC1,2,DEFAULT);
	VoC_movreg16(REG1,REG4,IN_PARALLEL);

	VoC_movreg16(REG2,REG4,DEFAULT);	

	VoC_lw16i_set_inc(REG0,g_FilterBank_MulBuf,1);


	VoC_lw16i(REG7,PROTOTYPE_FILTER_LEN);

	//REG0: g_FilterBank_MulBuf;
	//REG1: pTapppedDelayLine;
	//REG2: pTapppedDelayLine;
	//REG7: PROTOTYPE_FILTER_LEN;
#ifndef AEC_VOC_SYN_DEBUG
	//VoC_jal(Coolsand_AddVec);

	VoC_loop_r(0, REG7)
		VoC_lw16inc_p(REG6, REG0, DEFAULT);
		VoC_lw16i_short(REG7, 0, DEFAULT);
		VoC_bnltz16_r(SYN_FilterBank_ExtendTo32, REG6)
			VoC_lw16i(REG7, 0xffff);
SYN_FilterBank_ExtendTo32:
		VoC_add32inc_rp(REG67, REG67, REG1, DEFAULT);
		VoC_NOP();
		VoC_sw32inc_p(REG67, REG2, DEFAULT);
	VoC_endloop0
#else
// VoC_directive: PARSER_OFF
	{
		int i = 0;
		short *pFilterBankMulBuf = (short *)&RAM_X[(g_FilterBank_MulBuf-RAM_X_BEGIN_ADDR)/2];
		int *pTapppedDelayLine = (int *)&RAM_X[(r_OutSynFilterDelayLine-RAM_X_BEGIN_ADDR)/2];
		for (; i < PROTOTYPE_FILTER_LEN; i++)
		{
			int add1 = pFilterBankMulBuf[i];
			int add2 = pTapppedDelayLine[i];
			int temp = 0;

			if (add1 > 131071) // Q17
			{
				add1 = 131071;
				OverflowPrint(1732, "Coolsand_SynthesisProcessing");
			}
			else if (add1 < -131072)
			{
				add1 = -131072;
				OverflowPrint(1737, "Coolsand_SynthesisProcessing");
			}

			if (add2 > 131071) // Q17
			{
				add2 = 131071;
				OverflowPrint(1743, "Coolsand_SynthesisProcessing");
			}
			else if (add2 < -131072)
			{
				add2 = -131072;
				OverflowPrint(1748, "Coolsand_SynthesisProcessing");
			}

			temp = add1 + add2;
			if (temp > 131071) // Q17
			{
				temp = 131071;
				OverflowPrint(1755, "Coolsand_SynthesisProcessing");
			}
			else if (temp < -131072)
			{
				temp = -131072;
				OverflowPrint(1760, "Coolsand_SynthesisProcessing");
			}

			pTapppedDelayLine[i] = temp;
		}
	}
// VoC_directive: PARSER_ON
#endif

	VoC_movreg16(REG1,REG4,DEFAULT);
	VoC_movreg16(REG2,REG5,IN_PARALLEL);

	VoC_lw16i(REG7,SUBSAMPLING_RATE);

	VoC_lw16i_short(INC1,2,DEFAULT);
	VoC_lw16i_short(INC2,1,IN_PARALLEL);
	
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	VoC_push16(REG2, DEFAULT);
#endif
// VoC_directive: PARSER_ON

	//REG1: pTapppedDelayLine;
	//REG2: pOutData;
	//REG7: SUBSAMPLING_RATE;
	//pTapppedDelayLine is Q17, but pOutData is Q15, so we need to check
#ifndef AEC_VOC_SYN_DEBUG
	//VoC_jal(Coolsand_CopyVec);

	VoC_lw32_d(RL6, AEC_CONST_0x00007fff);
	VoC_lw32_d(RL7, AEC_CONST_0xffff8000);
	VoC_loop_r_short(REG7, DEFAULT)
	VoC_startloop0
		VoC_lw32inc_p(ACC0, REG1, DEFAULT);
		VoC_bngt32_rr(SYN_OUTDATA_SATURATE_MIN, ACC0, RL6)
			VoC_movreg32(ACC0, RL6, DEFAULT);
			VoC_jump(SYN_OUTDATA_SATURATE_END);
SYN_OUTDATA_SATURATE_MIN:
		VoC_bngt32_rr(SYN_OUTDATA_SATURATE_END, RL7, ACC0)
			VoC_movreg32(ACC0, RL7, DEFAULT);
			VoC_NOP();
SYN_OUTDATA_SATURATE_END:
		VoC_sw16inc_p(ACC0_LO, REG2, DEFAULT);
	VoC_endloop0
#else
// VoC_directive: PARSER_OFF
	{
		int i = 0;
		short *pDataOut = (short *)&RAM_X[(REGS[2].reg-RAM_X_BEGIN_ADDR)/2];
		int *pTapppedDelayLine = (int *)&RAM_X[(r_OutSynFilterDelayLine-RAM_X_BEGIN_ADDR)/2];
		for (; i < 8; i++)
		{
			int temp = pTapppedDelayLine[i];
			if (temp > 32767)
			{
				temp = 32767;
				OverflowPrint(1800, "Coolsand_SynthesisProcessing");
			}
			else if (temp < -32768)
			{
				temp = -32768;
				OverflowPrint(1800, "Coolsand_SynthesisProcessing");
			}
			pDataOut[i] = temp;
		}
	}
// VoC_directive: PARSER_ON
#endif
	
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_SYN_DEBUG
	VoC_pop16(REG2, DEFAULT);
	VoC_NOP();
	VoC_NOP();
	{
		short buffer[8] = {0};
		memcpy(buffer, &RAM_X[(REGS[2].reg-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "Final Ouput: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
	}
#endif
// VoC_directive: PARSER_ON

	VoC_movreg16(REG1,REG4,DEFAULT);
	VoC_lw16i_short(REG3,SUBSAMPLING_RATE*2,IN_PARALLEL);

	VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
	VoC_movreg16(REG2,REG4,IN_PARALLEL);

	VoC_lw16i_short(INC1,2,DEFAULT);
	VoC_lw16i_short(INC2,2,IN_PARALLEL);

	VoC_lw16i(REG7,(PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE)*2);

	//REG1: pTapppedDelayLine+SUBSAMPLING_RATE;
	//REG2: pTapppedDelayLine;
	//REG7: PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;
	VoC_jal(Coolsand_CopyVec);

	VoC_movreg16(REG1,REG4,DEFAULT);
	VoC_lw16i(REG3,(PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE)*2);

	VoC_add16_rr(REG1,REG1,REG3,DEFAULT);
	VoC_lw16i_short(REG7,SUBBAND_CHANNEL_NUM*2,IN_PARALLEL);

	//REG1: pTapppedDelayLine+PROTOTYPE_FILTER_LEN-SUBSAMPLING_RATE;
	//REG7: SUBSAMPLING_RATE;
	VoC_jal(Coolsand_ClearVec);

		
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_AdaptiveFiltering
 *
 * Description: Filtering Operation
 *
 * Inputs:       
 *   pMicData: pointer to Mic Data [In]
 *   pSpkData: pointer to Spk Data [In]
 *   pErrData: pointer to Output Data [Out]
 *   pSpkSigBuf: pointer to spk sig buffer [In/Out]
 *   pFilterBuf: pointer to filter [In]
 *   pFilterOut: filter output [Out]
 *   pOldSpkData: old spk data [Out]
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * NOTE:no
 *
 * Version 1.0  Created by  Guosz       07/29/2010
 **************************************************************************************/

void Coolsand_AdaptiveFilteringAmp(void)
{

	// Add new SpkData to Spk Sig Buffer

	//pOldSpkDataI->d = pSpkSigBufI[FILTER_LEN-1];
	//pOldSpkDataQ->d = pSpkSigBufQ[FILTER_LEN-1];

	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(REG7,FILTER_LEN-1,IN_PARALLEL);

	VoC_lw32_d(REG23,TEMP_r_AF_SpkSigBufIQ_addr_addr);
	
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

	VoC_lw32_d(REG45,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

	VoC_movreg32(REG01,REG23,DEFAULT);

	VoC_lw16_p(REG6,REG2,DEFAULT);
	VoC_lw16i_short(INC2,-1,IN_PARALLEL);

	VoC_lw16_p(REG7,REG3,DEFAULT);
	VoC_lw16i_short(INC3,-1,IN_PARALLEL);

	VoC_sw16_p(REG6,REG4,DEFAULT);
	VoC_inc_p(REG2,IN_PARALLEL);	

	VoC_sw16_p(REG7,REG5,DEFAULT);		
	VoC_inc_p(REG3,IN_PARALLEL);

	VoC_lw16i_short(INC0,-1,DEFAULT);
	VoC_lw16i_short(INC1,-1,IN_PARALLEL);
	
	VoC_lw32_d(REG67,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

//	BackCopyVec(pSpkSigBufI, pSpkSigBufI+1, FILTER_LEN-1);
//	BackCopyVec(pSpkSigBufQ, pSpkSigBufQ+1, FILTER_LEN-1);

	VoC_loop_i_short(FILTER_LEN-1,DEFAULT)	
	VoC_startloop0
		VoC_lw16inc_p(REG4,REG2,DEFAULT);
		VoC_lw16inc_p(REG5,REG3,DEFAULT);
		VoC_sw16inc_p(REG4,REG0,DEFAULT);
		VoC_sw16inc_p(REG5,REG1,DEFAULT);
	VoC_endloop0

	//pSpkSigBufI[0] = pSpkDataI->d;
	//pSpkSigBufQ[0] = pSpkDataQ->d;

	VoC_lw16_p(REG6,REG6,DEFAULT);
	VoC_lw16_p(REG7,REG7,DEFAULT);
	
	VoC_sw16_p(REG6,REG0,DEFAULT);
	VoC_lw16i_short(INC2,2,IN_PARALLEL);
	VoC_sw16_p(REG7,REG1,DEFAULT);
	VoC_lw16i_short(INC3,2,IN_PARALLEL);

	/*
	pFilterBufI: 32, 29, 21, 16, 14, 13, 9, 6
	pFilterBufQ: 0, 0, 0, 0, 0, 0, 0, 0
	g_AF_ShortFilterBufI: 65538, 65537, 0, 0, 0, 0, 0, 0
	g_AF_ShortFilterBufQ: 0, 0, 0, 0, 0, 0, 0, 0
	*/

	// Filter
	VoC_lw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);
//	TruncateVec(pFilterBufI, Filter_FixFormat, g_AF_ShortFilterBufI, ShortFilter_FixFormat, FILTER_LEN);
//	TruncateVec(pFilterBufQ, Filter_FixFormat, g_AF_ShortFilterBufQ, ShortFilter_FixFormat, FILTER_LEN);
	
//	ScaleVec( g_AF_ShortFilterBufQ,  g_AF_ShortFilterBufQ, -1, FILTER_LEN);
	VoC_lw16i_set_inc(REG0,g_AF_ShortFilterBufI,1);
	VoC_lw16i_set_inc(REG1,g_AF_ShortFilterBufQ,1);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_REF_DEBUG
	{
		int buffer[8] = {0};
		memcpy(buffer, &RAM_X[(REGS[2].reg-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "pFilterBufI: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
		memcpy(buffer, &RAM_X[(REGS[3].reg-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "pFilterBufQ: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
	}
#endif
	// VoC_directive: PARSER_ON
	
	VoC_loop_i_short(FILTER_LEN,DEFAULT);
	VoC_lw32z(REG45,IN_PARALLEL);
	VoC_startloop0
		VoC_lw32inc_p(ACC0,REG2,DEFAULT);
		VoC_lw32inc_p(REG67,REG3,DEFAULT);

		VoC_shr32_ri(ACC0,-12,DEFAULT);
		VoC_shr32_ri(REG67,-12,IN_PARALLEL);

		VoC_sub16_rr(REG7,REG5,REG7,DEFAULT);

		VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
		VoC_sw16inc_p(REG7,REG1,DEFAULT);
	VoC_endloop0
	VoC_sw32_d(REG23,TEMP_r_AF_FilterBufIQ_addr_addr);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_REF_DEBUG
	{
		short buffer[8] = {0};
		memcpy(buffer, &RAM_X[(g_AF_ShortFilterBufI-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "g_AF_ShortFilterBufI: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
		memcpy(buffer, &RAM_X[(g_AF_ShortFilterBufQ-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "g_AF_ShortFilterBufQ: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
		fflush(LogFileHandle);
	}
#endif
	// VoC_directive: PARSER_ON

/*	CmplxMulVec(	pSpkSigBufI, pSpkSigBufQ, SpkData_FixFormat, 
					g_AF_ShortFilterBufI, g_AF_ShortFilterBufQ, ShortFilter_FixFormat, 
					g_AF_tempCmplxBufI, g_AF_tempCmplxBufQ, FilterMul_FixFormat,
					FILTER_LEN, 
					ifFixRoundOff, 
					g_AF_tempBuf);
*/
//CmplxMulVec(int * pSrc0I, int * pSrc0Q, FixFormat Src0Format, int * pSrc1I, int * pSrc1Q, FixFormat Src1Format, 
			//int * pDstI, int *pDstQ, FixFormat DstFormat, int Len, bool ifRoundOff, int *tempBuf)
//	MulVec(	pSrc0I, Src0Format, pSrc1I, Src1Format, pDstI, MulFormat, Len, ifRoundOff);

//	ScaleVec( pSrc0Q,  pSrc0Q, -1, Len);
//	MulVec(	pSrc0Q, Src0Format, pSrc1Q, Src1Format, tempBuf, MulFormat, Len, ifRoundOff);
//	AddVec(pDstI, MulFormat, tempBuf, MulFormat, pDstI, DstFormat, Len, ifRoundOff);
//	ScaleVec( pSrc0Q,  pSrc0Q, -1, Len);

	VoC_lw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
	VoC_lw16i_set_inc(REG2,g_AF_ShortFilterBufI,2);
	VoC_lw16i_set_inc(REG3,g_AF_ShortFilterBufQ,2);

	VoC_lw16i(RL7_LO,g_AF_tempCmplxBufI);

	VoC_lw16i_short(INC0,2,DEFAULT);
	VoC_lw16i_short(INC1,2,IN_PARALLEL);
	
	VoC_lw32inc_p(REG45,REG2,DEFAULT);
	VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

	VoC_loop_i_short(FILTER_LEN/2,DEFAULT);
	VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
	VoC_startloop0

		VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
		VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);

// VoC_directive: PARSER_OFF
		VoC_NOP();
		VoC_NOP();
		VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
		if (REGL[0] > 262143)
			REGL[0] = 262143;
		else if (REGL[0] < -262144)
			REGL[0] = -262144;

		if (REGL[1] > 262143)
			REGL[1] = 262143;
		else if (REGL[1] < -262144)
			REGL[1] = -262144;
#endif
		VoC_NOP();
		VoC_NOP();
// VoC_directive: PARSER_ON


		VoC_lw16i_short(REG4,0,DEFAULT);

		VoC_sub16inc_rp(REG4,REG4,REG1,DEFAULT);
		VoC_sub16inc_rp(REG5,REG4,REG1,IN_PARALLEL);

		VoC_mac32_rr(REG6,REG4,DEFAULT);
		VoC_mac32_rr(REG7,REG5,IN_PARALLEL);

		VoC_lw32inc_p(REG67,REG3,DEFAULT);

		VoC_movreg16(REG3,RL7_LO,DEFAULT);
		VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

		VoC_lw32inc_p(REG45,REG2,DEFAULT);

// VoC_directive: PARSER_OFF
		VoC_NOP();
		VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
		if (REGL[0] == -262200 || REGL[1] == -262200)
			VoC_NOP();

		if (REGL[0] > 524287)
			REGL[0] = 524287;
		else if (REGL[0] < -524288)
			REGL[0] = -524288;

		if (REGL[1] > 524287)
			REGL[1] = 524287;
		else if (REGL[1] < -524288)
			REGL[1] = -524288;
#endif
		VoC_NOP();
		VoC_NOP();
// VoC_directive: PARSER_ON

		VoC_sw32inc_p(ACC0,REG3,DEFAULT);
		VoC_sw32inc_p(ACC1,REG3,DEFAULT);

		VoC_movreg16(REG3,RL7_LO,DEFAULT);
		VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

	VoC_endloop0

//	MulVec(	pSrc0I, Src0Format, pSrc1Q, Src1Format, pDstQ, MulFormat, Len, ifRoundOff);
//	MulVec(	pSrc0Q, Src0Format, pSrc1I, Src1Format, tempBuf, MulFormat, Len, ifRoundOff);
//	AddVec(pDstQ, MulFormat, tempBuf, MulFormat, pDstQ, DstFormat, Len, ifRoundOff);

	VoC_lw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);
	VoC_lw16i_set_inc(REG2,g_AF_ShortFilterBufQ,2);
	VoC_lw16i_set_inc(REG3,g_AF_ShortFilterBufI,2);

	VoC_lw16i(RL7_LO,g_AF_tempCmplxBufQ);

	VoC_lw16i_short(INC0,2,DEFAULT);
	VoC_lw16i_short(INC1,2,IN_PARALLEL);
	
	VoC_lw32inc_p(REG45,REG2,DEFAULT);
	VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

	VoC_loop_i_short(FILTER_LEN/2,DEFAULT);
	VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
	VoC_startloop0

		VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
		VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);

// VoC_directive: PARSER_OFF
		VoC_NOP();
		VoC_NOP();
		VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
		if (REGL[0] > 262143)
			REGL[0] = 262143;
		else if (REGL[0] < -262144)
			REGL[0] = -262144;

		if (REGL[1] > 262143)
			REGL[1] = 262143;
		else if (REGL[1] < -262144)
			REGL[1] = -262144;
#endif
		VoC_NOP();
		VoC_NOP();
// VoC_directive: PARSER_ON

		VoC_mac32inc_rp(REG6,REG1,DEFAULT);
		VoC_mac32inc_rp(REG7,REG1,IN_PARALLEL);

		VoC_lw32inc_p(REG67,REG3,DEFAULT);

		VoC_movreg16(REG3,RL7_LO,DEFAULT);
		VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

		VoC_lw32inc_p(REG45,REG2,DEFAULT);

// VoC_directive: PARSER_OFF
		VoC_NOP();
		VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
		if (REGL[0] > 524287)
			REGL[0] = 524287;
		else if (REGL[0] < -524288)
			REGL[0] = -524288;

		if (REGL[1] > 524287)
			REGL[1] = 524287;
		else if (REGL[1] < -524288)
			REGL[1] = -524288;
#endif
		VoC_NOP();
		VoC_NOP();
// VoC_directive: PARSER_ON
		
		VoC_sw32inc_p(ACC0,REG3,DEFAULT);
		VoC_sw32inc_p(ACC1,REG3,DEFAULT);

		VoC_movreg16(REG3,RL7_LO,DEFAULT);
		VoC_movreg16(RL7_LO,REG3,IN_PARALLEL);

	VoC_endloop0

	VoC_sw32_d(REG01,TEMP_r_AF_SpkSigBufIQ_addr_addr);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_REF_DEBUG
	{
		int buffer[8] = {0};
		memcpy(buffer, &RAM_X[(g_AF_tempCmplxBufI-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "g_AF_tempCmplxBufI: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
		memcpy(buffer, &RAM_X[(g_AF_tempCmplxBufQ-RAM_X_BEGIN_ADDR)/2], sizeof(buffer));
		fprintf(LogFileHandle, "g_AF_tempCmplxBufQ: %d, %d, %d, %d, %d, %d, %d, %d\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
			buffer[6], buffer[7]);
	}
#endif
	// VoC_directive: PARSER_ON

//	VecSum(g_AF_tempCmplxBufI, FilterMul_FixFormat, pFilterOutI, FILTER_LEN, ifFixRoundOff);
//	VecSum(g_AF_tempCmplxBufQ, FilterMul_FixFormat, pFilterOutQ, FILTER_LEN, ifFixRoundOff);
	VoC_lw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);
	VoC_lw16i_set_inc(REG0,g_AF_tempCmplxBufI,2);
	VoC_lw16i_set_inc(REG1,g_AF_tempCmplxBufQ,2);
	
	VoC_lw16i_short(INC2,1,DEFAULT);
	VoC_lw16i_short(INC3,1,IN_PARALLEL);

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw32z(ACC1,IN_PARALLEL);

	VoC_loop_i_short(FILTER_LEN,DEFAULT);
	VoC_startloop0
		VoC_add32inc_rp(ACC0,ACC0,REG0,DEFAULT);
		VoC_shr32_ri(ACC1,11,IN_PARALLEL);

		VoC_add32inc_rp(ACC1,ACC1,REG1,DEFAULT);
		VoC_shr32_ri(ACC0,-11,IN_PARALLEL);

		VoC_shr32_ri(ACC0,11,DEFAULT);
		VoC_shr32_ri(ACC1,-11,IN_PARALLEL);		
		
	VoC_endloop0


	VoC_lw16i_short(FORMAT16,5-16,DEFAULT);

	VoC_lw16i(REG0,r_AF_FilterAmpI);
	
	VoC_add16_rd(REG0,REG0,g_ch);
	
	VoC_shr32_ri(ACC0,-12,DEFAULT);
	VoC_shr32_ri(ACC1,-1,IN_PARALLEL);		

	VoC_movreg16(REG6,ACC0_HI,DEFAULT);
	VoC_movreg16(REG7,ACC1_HI,IN_PARALLEL);

	VoC_multf16_rp(REG6,REG6,REG0,DEFAULT);
	VoC_multf16_rp(REG7,REG7,REG0,IN_PARALLEL);

	VoC_lw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);

	VoC_lw16i_short(INC0,1,DEFAULT);
	VoC_lw16i_short(INC1,1,IN_PARALLEL);

	VoC_sw16inc_p(REG6,REG2,DEFAULT);	// pFilterOutI

	VoC_sw16inc_p(REG7,REG3,DEFAULT);	// pFilterOutQ
	
	
//	tempFixI = (*pMicDataI) -(*pFilterOutI);
//	pErrDataI->d = tempFixI.d;
//	tempFixQ = (*pMicDataQ) -(*pFilterOutQ);
//	pErrDataQ->d = tempFixQ.d;
	
	VoC_lw32_d(REG23,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
	VoC_sw32_d(REG23,TEMP_g_AF_FilterOutBufIQ_addr_addr);

	VoC_sub16inc_pr(REG4,REG2,REG6,DEFAULT);
	VoC_sub16inc_pr(REG5,REG3,REG7,DEFAULT);
	VoC_sw16inc_p(REG4,REG0,DEFAULT);
	VoC_sw16inc_p(REG5,REG1,DEFAULT);
	VoC_sw32_d(REG23,TEMP_g_MicAnaFilterOutBufIQ_addr_addr);
	VoC_pop16(RA,DEFAULT);
	VoC_sw32_d(REG01,TEMP_g_AF_ErrDataBufIQ_addr_addr);
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_UpdateSpkPower
 *
 * Description: Update Spk Power Estimation
 *
 * Inputs:       
 *   pSpkSigBuf: pointer to spk sig buffer [In]
 *   pOldSpkData: old spk data [In]
 *   pSpkSigPower: spk power estimation [In/Out]
 *
 * Outputs: no.
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Guosz       07/29/2010
 **************************************************************************************/
void Coolsand_UpdateSpkPower(void)
{
	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(INC0,1,DEFAULT);
	VoC_lw16i_short(INC1,1,IN_PARALLEL);
		//UpdateSpkPower(	&SubSpkDataI, &SubSpkDataQ, 
		//	r_AF_OldSpkDataBufI+ch, r_AF_OldSpkDataBufQ+ch, r_AF_SpkSigPower+ch);
//	CmplxCutNorm(pSpkDataI, pSpkDataQ, &NewSpkDataNorm);
	VoC_lw32_d(REG01,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);
	VoC_lw16i_short(INC2,1,DEFAULT);
	VoC_lw16i_short(INC3,1,IN_PARALLEL);

	VoC_lw16inc_p(REG4,REG0,DEFAULT);
	VoC_lw16i_short(FORMAT32,6,IN_PARALLEL);

	VoC_lw16inc_p(REG5,REG1,DEFAULT);
	VoC_multf32_rr(ACC0,REG4,REG4,IN_PARALLEL);
	VoC_multf32_rr(ACC1,REG5,REG5,DEFAULT);
	VoC_lw32_d(REG23,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

	VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);		// ACC0 for NewSpkDataNorm
//	CmplxCutNorm(pOldSpkDataI, pOldSpkDataQ, &OldSpkDataNorm);
	VoC_lw16inc_p(REG4,REG2,DEFAULT);
	VoC_shr32_ri(ACC0,-6,IN_PARALLEL);
	
	VoC_lw16inc_p(REG5,REG3,DEFAULT);
	VoC_multf32_rr(RL6,REG4,REG4,IN_PARALLEL);

	VoC_multf32_rr(RL7,REG5,REG5,DEFAULT);
	VoC_shr32_ri(ACC0,6,IN_PARALLEL);

	VoC_sw32_d(REG01,TEMP_g_SpkAnaFilterOutBufIQ_addr_addr);

	VoC_add32_rr(RL6,RL6,RL7,DEFAULT);			// rl6 for OldSpkDataNorm
		
	VoC_lw16d_set_inc(REG2,TEMP_r_AF_SpkSigPower_addr_addr,2);
	VoC_sw32_d(REG23,TEMP_r_AF_OldSpkDataBufIQ_addr_addr);

	VoC_shr32_ri(RL6,-6,DEFAULT);
	VoC_lw32_p(ACC1,REG2,IN_PARALLEL);

	
	VoC_shr32_ri(RL6,6,DEFAULT);
	VoC_shr32_ri(ACC1,-1,IN_PARALLEL);
//	TempSpkPowerFix = NewSpkDataNorm-OldSpkDataNorm;
	
	VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);

//	TempSpkPowerFix = TempSpkPowerFix+(*pSpkSigPower);
//	Truncate(TempSpkPowerFix, pSpkSigPower->t, pSpkSigPower->f);
	

	VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);


	VoC_shr32_ri(ACC0,1,DEFAULT);

	VoC_shr32_ri(ACC0,-2,DEFAULT);
	VoC_shr32_ri(ACC0,2,DEFAULT);
	
//	if (pSpkSigPower->d<0)
	VoC_bnltz32_r(Coolsand_UpdateSpkPower100,ACC0)
//		pSpkSigPower->d = 0;
		VoC_lw32z(ACC0,DEFAULT);

Coolsand_UpdateSpkPower100:
	//	pSpkSigPower->d = TempSpkPowerFix.d;
	VoC_pop16(RA,DEFAULT);
	VoC_sw32inc_p(ACC0,REG2,DEFAULT);
	VoC_sw16_d(REG2,TEMP_r_AF_SpkSigPower_addr_addr);
	
	VoC_return;


}



/**************************************************************************************
 * Function:    Coolsand_FilterUpdateCotrol_Int
 *
 * Description: no
 *
 * Inputs:       
 *   REG2: SubSpkDataI  mean pSpkDataI
 *   REG3: SubSpkDataQ  mean pSpkDataQ
 *	             data format: Q15
 *	 REG6: m_AF_FrameCnt  
 *	 REG7: ch
 *
 * Outputs: 
 *	 REG3:
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 1.1  optimized by  Cuilf     08/14/2010
 * Version 1.2  fixed by  Xuml     08/26/2010
 **************************************************************************************/

void Coolsand_FilterUpdateCotrol_Int(void)
{

	//
	// Err Data and Spk Data
	//
  
    //Input Data format:    pSpkDataI&pSpkDataQ , pErrDataI, pErrDataQ  are Q15
    //                      r_AF_RErrSpkI_addr  ,r_AF_RErrSpkQ_addr     are Q23
     VoC_push16(RA,DEFAULT);


     VoC_lw16i(REG0,g_AF_ErrDataBufI);
     VoC_lw16i(REG1,g_AF_ErrDataBufQ);

     VoC_movreg16(REG6,REG7,DEFAULT); //ch
     VoC_push16(REG6,IN_PARALLEL);//save m_AF_FrameCnt 

     VoC_add32_rr(REG01,REG01,REG67,DEFAULT); //ErrDataI = pErrDataI->d;  //ErrDataQ = pErrDataQ->d;
	 
	// Conj Err Data

	//
	// Compute Coupling Factor
	//
	// Update RErrSpk
 

    VoC_lw16i_short(FORMAT32,0,DEFAULT);//InstantCorrI = InstantCorrI>>(ErrFracBitNum+SpkFracBitNum-RErrSpkFracBitNum);
    VoC_multf32_rp(ACC0, REG2,REG0,DEFAULT); // InstantCorrI = ConjErrDataI*SpkDataI-ConjErrDataQ*SpkDataQ; 
    VoC_multf32_rp(ACC1,REG3,REG0,IN_PARALLEL);//InstantCorrQ = ConjErrDataI*SpkDataQ+ConjErrDataQ*SpkDataI;

	VoC_mac32_rp(REG3,REG1,DEFAULT);    // 	ConjErrDataQ = -pErrDataQ->d;
	VoC_msu32_rp(REG2,REG1,IN_PARALLEL);//	ConjErrDataQ = -pErrDataQ->d;   InstantCorrQ = InstantCorrQ>>(ErrFracBitNum+SpkFracBitNum-RErrSpkFracBitNum);

	VoC_NOP();
	VoC_shr32_ri(ACC0,7,DEFAULT);
	VoC_shr32_ri(ACC1,7,IN_PARALLEL);
                                      //After multf, the data format is Q23
    VoC_push16(REG7,DEFAULT);//save ch 
	VoC_lw16i_short(REG5,0x0008,IN_PARALLEL);

    VoC_movreg32(RL7,ACC0,DEFAULT);//RL7 :InstCorrI_Abs;
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

	VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrI_abs,ACC0)
		VoC_lw32z(RL7,DEFAULT);
		VoC_sub32_rr(RL7,RL7,ACC0,DEFAULT);//RL7 :InstCorrI_Abs;
Coolsand_FilterUpdateCotrol_Int_InstantCorrI_abs:
 
    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCorrI,REG45,RL7)
        VoC_movreg32(ACC0,REG45,DEFAULT);
		VoC_movreg32(RL7,ACC0,IN_PARALLEL);
		VoC_NOP();
		VoC_bgtz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrI,RL7)
			VoC_lw16i_short(ACC0_HI,0xfff8,DEFAULT);
			VoC_lw16i_short(ACC0_LO,0x0,IN_PARALLEL);
Coolsand_FilterUpdateCotrol_Int_InstantCorrI:
   //ACC0  InstantCorrI

	VoC_movreg32(RL6,ACC1,DEFAULT);//RL6 :InstCorrQ_Abs;
   	VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ_abs,ACC1)
		VoC_lw32z(RL6,DEFAULT);
		VoC_sub32_rr(RL6,RL6,ACC1,DEFAULT);//RL6 :InstCorrQ_Abs;
Coolsand_FilterUpdateCotrol_Int_InstantCorrQ_abs:
 
    VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ,REG45,RL6)
		VoC_movreg32(ACC1,REG45,DEFAULT);
		VoC_movreg32(RL6,ACC1,IN_PARALLEL);
		VoC_NOP();
		VoC_bgtz32_r(Coolsand_FilterUpdateCotrol_Int_InstantCorrQ,RL6)
			VoC_lw16i_short(ACC1_HI,0xfff8,DEFAULT);
			VoC_lw16i_short(ACC1_LO,0x0,IN_PARALLEL);
Coolsand_FilterUpdateCotrol_Int_InstantCorrQ:
    //ACC1  InstantCorrQ

	// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
		  VoC_NOP();
		  VoC_NOP();
		  fprintf(LogFileHandle, "InstantCorrI: %d\tInstantCorrQ: %d\n", REGL[0], REGL[1]);
		}
	#endif
	// VoC_directive: PARSER_ON 

    VoC_add16_rr(REG6,REG7,REG7,DEFAULT);
	
    VoC_lw16i(REG4,r_AF_RErrSpkI);
    VoC_lw16i(REG5,r_AF_RErrSpkQ);


	VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
	VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

	 VoC_push32(REG23,DEFAULT);
     VoC_lw16i_short(FORMAT32,15,IN_PARALLEL); 

     VoC_lw32_p(RL7,REG4,DEFAULT);
     VoC_lw16i_short(FORMATX,0,IN_PARALLEL); 

     VoC_lw32_p(RL6,REG5,DEFAULT);
//RL6 : pRErrSpkQ->d  == LastCorrQ
//RL7: pRErrSpkI->d  == LastCorrI
//ACC0  InstantCorrI    ACC1  InstantCorrQ 
//Those all are Q23
    
	 VoC_push16(REG7,DEFAULT);
	 VoC_push32(REG01,IN_PARALLEL);

	 VoC_movreg32(REG01,ACC0,DEFAULT);
	 VoC_movreg32(REG23,ACC1,IN_PARALLEL);

	 VoC_lw32z(REG67,DEFAULT);
	 
   	VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_L0,REG01)
		VoC_sub32_rr(REG01,REG67,REG01,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_L0:    

   	VoC_bnltz32_r(Coolsand_FilterUpdateCotrol_Int_L1,REG23)
		VoC_sub32_rr(REG23,REG67,REG23,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_L1:   
	
	VoC_lw16i_short(REG6,1,DEFAULT);

     VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_L2,REG01,REG67)
     VoC_bngt32_rr(Coolsand_FilterUpdateCotrol_Int_L3,REG23,REG67)//		if ( InstCorrI_Abs >1 || InstCorrQ_Abs >1 )
Coolsand_FilterUpdateCotrol_Int_L2:

     VoC_lw16_d(REG6,m_AECProcPara_AFUpdateFactor);

	 VoC_mult16_rd(REG6,REG6,m_AECProcPara_AFCtrlSpeedUpFactor);
	 
	/*
		 int NewValueI, NewValueQ;
		 NewValueI = REGL[0] - REGL[3];
		 NewValueI = (NewValueI * REGS[6].reg) >> 15;
		 REGL[0] = REGL[3] + NewValueI;

		 NewValueQ = REGL[1] - REGL[2];
		 NewValueQ = (NewValueQ * REGS[6].reg) >> 15;
		 REGL[1] = REGL[2] + NewValueQ;
	*/

	VoC_sub32_rr(REG01,ACC0,RL7,DEFAULT);// 	NewValueI= InstantCorrI - pRErrSpkI->d;
	VoC_sub32_rr(REG23,ACC1,RL6,IN_PARALLEL);    // 	NewValueQ= InstantCorrQ - pRErrSpkQ->d; 

	VoC_shr32_ri(REG01, -1, DEFAULT);
	VoC_shr32_ri(REG23, -1, IN_PARALLEL);

	VoC_shru16_ri(REG0, 1, DEFAULT);
	VoC_shru16_ri(REG2, 1, IN_PARALLEL);
		 
	VoC_multf32_rr(ACC0, REG0, REG6, DEFAULT);
	VoC_multf32_rr(ACC1, REG2, REG6, IN_PARALLEL);
		 
	VoC_macX_rr(REG1, REG6, DEFAULT);
	VoC_macX_rr(REG3, REG6, IN_PARALLEL);

	VoC_NOP();

	VoC_add32_rr(ACC0, ACC0, RL7, DEFAULT);
	VoC_add32_rr(ACC1, ACC1, RL6, IN_PARALLEL);

	VoC_NOP();

	 /*
     VoC_sub32_rr(REG23,ACC0,RL7,DEFAULT);// 	NewValueI= InstantCorrI - pRErrSpkI->d;

     VoC_sub32_rr(REG01,ACC1,RL6,DEFAULT);    // 	NewValueQ= InstantCorrQ - pRErrSpkQ->d; 
	 VoC_multf32_rr(ACC0,REG3,REG6,IN_PARALLEL);//AFUpdateFactor is Q15, Q15xQ23 >>8 is Q23
	 
	 //acc1 : NewValueI
     VoC_shru16_ri(REG2,1,DEFAULT); //    NewValueI= (NewValueI*(UpdateFactor.d)) >>UpdateFactor.f;
 	 VoC_multf32_rr(ACC1,REG1,REG6,IN_PARALLEL);//AFUpdateFactor is Q15, Q15xQ15 >>8 is Q23
    
     VoC_macX_rr(REG2,REG6,DEFAULT);
     VoC_shru16_ri(REG0,1,DEFAULT); // Q23->Q15 in reg3   NewValueQ= (NewValueQ*(UpdateFactor.d)) >>UpdateFactor.f;
      
	 VoC_shr32_ri(ACC0,15,DEFAULT);
	 
     VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);//pRErrSpkI->d = pRErrSpkI->d +NewValueI;
     VoC_macX_rr(REG0,REG6,IN_PARALLEL);

	 VoC_NOP();
     
     VoC_shr32_ri(ACC1,15,DEFAULT);

     VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT); // 	pRErrSpkQ->d = pRErrSpkQ->d +NewValueQ;
	 */
	 
     VoC_sw32_p(ACC0,REG4,DEFAULT);
	 
     VoC_sw32_p(ACC1,REG5,DEFAULT);

Coolsand_FilterUpdateCotrol_Int_L3:
	 
	// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
		  int *pRErrSpkI = &RAM_X[(r_AF_RErrSpkI-RAM_X_BEGIN_ADDR)/2];
		  int *pRErrSpkQ = &RAM_X[(r_AF_RErrSpkQ-RAM_X_BEGIN_ADDR)/2];
		  int ch = *((short *)&RAM_X[(g_ch-RAM_X_BEGIN_ADDR)/2]+1);
		  VoC_NOP();
		  VoC_NOP();
		  fprintf(LogFileHandle, "pRErrSpkI: %d\tpRErrSpkQ: %d\n", pRErrSpkI[ch], pRErrSpkQ[ch]);
		}
	#endif
	// VoC_directive: PARSER_ON 

       VoC_lw16_d(REG6,m_AECProcPara_AFUpdateFactor);

	   VoC_pop32(REG01,DEFAULT);
	   VoC_pop16(REG7,IN_PARALLEL);

       VoC_pop32(REG23,DEFAULT);
       VoC_push32(REG45,DEFAULT);
//RL6 : pRErrSpkQ->d  == LastCorrQ
//RL7: pRErrSpkI->d  == LastCorrI

//Those all are Q23


	// Update Spk Sig Smooth Power
       VoC_lw16i(REG4,r_AF_SpkSigSmoothPower);

       VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
	   VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);// 	int tempMul0 = (SpkDataI*SpkDataI)>>(SpkFracBitNum*2-SpkSmoothPowFracBitNum);
	   
       VoC_multf32_rr(ACC0,REG2,REG2,DEFAULT);//int tempMul1 = (SpkDataQ*SpkDataQ)>>(SpkFracBitNum*2-SpkSmoothPowFracBitNum);
	  VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);

	   VoC_mac32_rr(REG3,REG3,DEFAULT);  // 	SpkDataPower = tempMul0+tempMul1;
	   VoC_lw16i_short(FORMATX,15,IN_PARALLEL);
	   VoC_lw32_p(ACC1,REG4,DEFAULT);
	   //ACC1:  LastSpkSmoothPower = pSpkSigSmoothPower->d;

		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
		  VoC_NOP();
		  VoC_NOP();
		  fprintf(LogFileHandle, "SpkDataPower: %d\n", REGL[0]);
		}
	#endif
		// VoC_directive: PARSER_ON

     VoC_bez32_r(Coolsand_FilterUpdateCotrol_Int_SpkDataPower,ACC0)//	if (SpkDataPower!=0)
	   VoC_lw16i_short(FORMAT32,0,DEFAULT); 
       VoC_sub32_rr(REG23,ACC0,ACC1,DEFAULT);//NewValue = SpkDataPower - pSpkSigSmoothPower->d;
	   VoC_shr32_ri(REG23,-1,DEFAULT);
       VoC_multf32_rr(ACC0,REG3,REG6,DEFAULT);//NewValue = (NewValue*(UpdateFactor.d)) >>UpdateFactor.f;
       VoC_shru16_ri(REG2,1,IN_PARALLEL);   // Q23->Q15 in reg3  
	   
       VoC_macX_rr(REG2,REG6,DEFAULT);
       VoC_NOP();
	   //VoC_shr32_ri(ACC0,15,DEFAULT);
	   VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);//pSpkSigSmoothPower->d = pSpkSigSmoothPower->d+NewValue;
       VoC_NOP();
	   VoC_sw32_p(ACC0,REG4,DEFAULT);

Coolsand_FilterUpdateCotrol_Int_SpkDataPower:
	    VoC_pop32(REG45,DEFAULT);
		VoC_lw32_p(ACC0,REG4,DEFAULT);
		VoC_push32(ACC1,DEFAULT);//push LastSpkSmoothPower
        VoC_push32(ACC0,DEFAULT);//push pSpkSigSmoothPower->d    
     VoC_lw32_p(ACC0,REG4,DEFAULT);// ACC0: pRErrSpkI->d 
     VoC_lw32_p(ACC1,REG5,DEFAULT);//ACC1 : pRErrSpkQ->d
    
	// Compute RErrSpk Norm

    VoC_jal(Coolsand_CmplxAppxNorm_int); // 	CmplxAppxNorm( (int *) &(pRErrSpkI->d), (int *)&(pRErrSpkQ->d), &RErrSpkNorm );
      VoC_shr32_ri(ACC0,8,DEFAULT);  // 	RErrSpkNorm = RErrSpkNorm >> (RErrSpkFracBitNum-CFFracBitNum);
	  VoC_movreg32(ACC1,RL6,IN_PARALLEL);

      VoC_pop32(RL6,DEFAULT);//pSpkSigSmoothPower->d 
	  VoC_pop32(REG23,DEFAULT);//LastSpkSmoothPower

		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
		  VoC_NOP();
		  VoC_NOP();
		  fprintf(LogFileHandle, "LastSpkSmoothPower: %d\npSpkSigSmoothPower: %d\n",
			  (REGS[3].reg << 16) + REGS[2].reg, REGL[2]);
		}
	#endif
		// VoC_directive: PARSER_ON

    VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm,ACC0)

// RL7:  pRErrSpkI->d  == LastCorrI
//RL6 : pRErrSpkQ->d  == LastCorrQ
       VoC_movreg32(ACC0,RL7,DEFAULT);

       VoC_jal(Coolsand_CmplxAppxNorm_int);// CmplxAppxNorm( &LastCorrI, &LastCorrQ, &RErrSpkNorm );
       VoC_shr32_ri(ACC0,8,DEFAULT);  //	RErrSpkNorm = RErrSpkNorm >> (RErrSpkFracBitNum-CFFracBitNum);
       VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm_100,ACC0)  
       VoC_lw16i_short(ACC0_LO,2,DEFAULT);
Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm_100:
       VoC_movreg32(RL6,REG23,DEFAULT); //LastSpkSmoothPower   SpkSigSmoothPower_r = LastSpkSmoothPower*FilterFactor;

Coolsand_FilterUpdateCotrol_Int_RErrSpkNorm:
	   
       VoC_movreg32(REG23,RL6,DEFAULT);
	   VoC_shr32_ri(RL6,-1,DEFAULT);      //FilterFactor =3

	   VoC_add32_rr(RL6,RL6,REG23,DEFAULT);// SpkSigSmoothPower_r = pSpkSigSmoothPower->d*FilterFactor; 
       VoC_shr32_ri(RL6,12,DEFAULT);// 	SpkSigSmoothPower_r = SpkSigSmoothPower_r>> (SpkSmoothPowFracBitNum-SpkSthPowerFracBitNum);

	// Coupling factor
       VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_SpkSigSmoothPower_r,RL6)//	if (SpkSigSmoothPower_r == 0)
           VoC_lw16i_short(RL6_LO,1,DEFAULT);                       //    SpkSigSmoothPower_r = 1;
Coolsand_FilterUpdateCotrol_Int_SpkSigSmoothPower_r:

	     VoC_lw16i_short(REG2,12,DEFAULT);//xuml
		 VoC_push32(ACC0, DEFAULT);
		 VoC_push32(RL6, DEFAULT);
		 VoC_jal(Coolsand_MSBPos);  //	int SpkSmoothPowerFirstBitPos = MSBPos(SpkSigSmoothPower_r, SpkSthPowerFracBitNum);
         VoC_pop32(RL6, DEFAULT);
		 VoC_pop32(ACC0, DEFAULT);
		 //REG3: SpkSmoothPowerFirstBitPos
		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
		  VoC_NOP();
		  VoC_NOP();
		  fprintf(LogFileHandle, "SpkSigSmoothPower_r: %d\nSpkSmoothPowerFirstBitPos: %d\nRErrSpkNorm: %d\n",
			  REGL[2], REGS[3].reg, REGL[0]);
		}
	#endif
		// VoC_directive: PARSER_ON

		  VoC_bnez16_r(Coolsand_FilterUpdateCotrol_Int_CouplingFactor_Init, REG3)
			  VoC_lw32_d(ACC0, AEC_CONST_0x00007fff);
			  VoC_jump(Coolsand_FilterUpdateCotrol_Int_CouplingFactor_Init_END);
Coolsand_FilterUpdateCotrol_Int_CouplingFactor_Init:
			  VoC_lw16i_short(REG2,12,DEFAULT);
			  VoC_sub16_rr(REG2,REG3,REG2,DEFAULT);//  -(SpkSthPowerFracBitNum+1-SpkSmoothPowerFirstBitPos)
			  VoC_shr32_rr(ACC0,REG2,DEFAULT);// 	CouplingFactor = RErrSpkNorm << (SpkSthPowerFracBitNum+1-SpkSmoothPowerFirstBitPos);
Coolsand_FilterUpdateCotrol_Int_CouplingFactor_Init_END:

   //        VoC_lw16i_short(REG5,0,IN_PARALLEL);
	//	  VoC_lw16i(REG4,0x8000);
        
    //   VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_CouplingFactor,REG45,ACC0)  //	if (CouplingFactor >= (0x1<<CFFracBitNum) )
     //     VoC_lw32z(ACC0,DEFAULT);
	 //     VoC_lw16i(ACC0_LO,0x7fff);                             // 	CouplingFactor = (0x1<<CFFracBitNum)-1; 
//Coolsand_FilterUpdateCotrol_Int_CouplingFactor:

		
		VoC_shr32_ri(ACC0,-1,DEFAULT);
		
        VoC_lw16_p(ACC1_HI,REG1,DEFAULT);//ErrDataQ = pErrDataQ->d;      
	    VoC_push32(ACC0,DEFAULT);//CouplingFactor 

	//
	// Instant Coupling Factor
	//

	// Err Norm
    
    VoC_lw16_p(ACC0_HI,REG0,DEFAULT);//ErrDataI = pErrDataI->d;
 
    VoC_shr32_ri(ACC0,16,DEFAULT);
    VoC_shr32_ri(ACC1,16,IN_PARALLEL);

	VoC_jal(Coolsand_CmplxAppxNorm_int);   // 	CmplxAppxNorm(&ErrDataI, &ErrDataQ, &ErrDataNorm);
    // ACC0 :  ErrDataNorm

	// Sqrt Spk Sig Power


     VoC_lw16i(REG0,r_AF_SpkSigPower);//r_AF_SpkSigPower_addr : Q3.23
     VoC_add16_rr(REG6,REG7,REG7,DEFAULT);  //REG7:ch
     VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

     VoC_lw16i(REG4,0x800);
	 VoC_lw32_p(RL6,REG0,DEFAULT);

     VoC_shr32_ri(RL6,12,DEFAULT);// 	SpkSigPower_s = pSpkSigPower->d >> (pSpkSigPower->f - SpkPowShortFracBitNum);
     VoC_lw16i_short(REG5,0,IN_PARALLEL);

     VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s,REG45,RL6)// if (SpkSigPower_s>= (0x1<<(SpkPowShortTotalBitNum-1)))
		VoC_lw16i(RL6_LO,0x7ff);                                               //	SpkSigPower_s = (0x1<<(SpkPowShortTotalBitNum-1))-1;
		VoC_lw16i_short(RL6_HI,0,DEFAULT);

 Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s:
     VoC_bnez32_r(Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s_100,RL6)  //if (SpkSigPower_s == 0)
        VoC_lw16i_short(RL6_LO,1,DEFAULT);                       // 	SpkSigPower_s = 1;
Coolsand_FilterUpdateCotrol_Int_SpkSigPower_s_100:

	
         VoC_lw16i_short(REG2,12,DEFAULT);//xuml
        VoC_jal(Coolsand_MSBPos); // int SpkPowerFirstBitPos = MSBPos( SpkSigPower_s,SpkPowShortTotalBitNum-1);

	// Div
       VoC_lw16i_short(REG7,12,DEFAULT);
	   VoC_sub16_rr(REG4,REG7,REG3,DEFAULT);
	   VoC_lw16i_short(REG6,1,IN_PARALLEL);
       VoC_and16_rr(REG4,REG6,DEFAULT);

	   
	VoC_bez16_r(Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos,REG4)// if ((SpkPowerFirstBitPos-1-SpkPowShortFracBitNum)%2 != 0)
        VoC_lw16i_short(REG4,11,DEFAULT);
        VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
		VoC_shr16_ri(REG4,1,DEFAULT);
		VoC_sub16_rr(REG4,REG6,REG4,DEFAULT); //	SpkPowerShiftValue = -((SpkPowerFirstBitPos-SpkPowShortFracBitNum)/2+1);
		//REG4 : SpkPowerShiftValue
	
        VoC_shr32_rr(ACC0,REG4,DEFAULT);
	
        VoC_movreg32(REG23,ACC0,DEFAULT);
		VoC_shr32_ri(ACC0,-1,DEFAULT);
		VoC_add32_rr(ACC0,ACC0,REG23,DEFAULT);// InstantCouplingFactor = InstantCouplingFactor*3;


		VoC_jump(Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos_end)


Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos:

         VoC_sub16_rr(REG4,REG3,REG7,DEFAULT);
		 VoC_shr16_ri(REG4,1,DEFAULT);   //	SpkPowerShiftValue = -(SpkPowerFirstBitPos-1-SpkPowShortFracBitNum)/2;
         VoC_shr32_rr(ACC0,REG4,DEFAULT);//InstantCouplingFactor = ErrDataNorm << SpkPowerShiftValue;

Coolsand_FilterUpdateCotrol_Int_SpkPowerFirstBitPos_end:
       VoC_lw16i_short(ACC1_LO,0,DEFAULT);
       VoC_lw16i(ACC1_HI,0x0010);
 
	VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_InstantCouplingFactor,ACC1,ACC0)//	if (InstantCouplingFactor>= (0x1<<(InstantCFTotalBitNum-1)) )
    
       VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
       VoC_lw16i_short(ACC0_HI,0xf,DEFAULT);//0x000fffff                                      //	InstantCouplingFactor = (0x1<<(InstantCFTotalBitNum-1))-1; 
	
Coolsand_FilterUpdateCotrol_Int_InstantCouplingFactor:

	  VoC_pop16(REG7,DEFAULT);//restore ch

       
   	  VoC_lw16i(REG0,g_AF_StepSize);

	  VoC_add16_rr(REG0,REG0,REG7,DEFAULT);//fStepSize = 1.0;
      VoC_pop16(REG6,IN_PARALLEL);//restore m_AF_FrameCnt


	  VoC_pop32(ACC1,DEFAULT);//CouplingFactor
      VoC_lw16i_short(REG2,8,IN_PARALLEL);

	  VoC_lw16i(REG5,0x7f);//	*pVarStepSize = Double2Fix(fStepSize, pVarStepSize->t, pVarStepSize->f);
	  
		// VoC_directive: PARSER_OFF
	#ifdef AEC_VOC_REF_DEBUG
		{
			VoC_NOP();
			VoC_NOP();
			fprintf(LogFileHandle, "CouplingFactor: %d\nInstanceCouplingFactor: %d\n", REGL[1], REGL[0]);
		}
	#endif
		// VoC_directive: PARSER_ON	  

	  VoC_bgt16_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end,REG2,REG6) //	if (FrameCnt<FILTER_LEN)
	  VoC_bgt32_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end, ACC1,ACC0)// if (CouplingFactor> ( InstantCouplingFactor<<(CFFracBitNum-InstantCFFracBitNum) ))

             //input : RL6, REG2
            //output: REG3
            VoC_movreg32(RL6,ACC0,DEFAULT);
	        VoC_lw16i_short(REG2,21,DEFAULT);//xuml
			VoC_jal(Coolsand_MSBPos);                    //	int InstantCFFirstBitPos = MSBPos(InstantCouplingFactor, InstantCFTotalBitNum-1);
            VoC_lw16i_short(REG2,16,DEFAULT);
			VoC_sub16_rr(REG3,REG3,REG2,DEFAULT);//int StepShiftVal = InstantCFFracBitNum+1-InstantCFFirstBitPos;
			VoC_shr32_rr(ACC1,REG3,DEFAULT);  //	StepSize_Int = CouplingFactor >> (-StepShiftVal);
				
			VoC_shr32_ri(ACC1,8,DEFAULT); // 	StepSize_Int = StepSize_Int >> (CFFracBitNum - pVarStepSize->f);
		 	VoC_movreg16(REG5,ACC1_LO,DEFAULT);	
            VoC_lw16i(REG4,0x80);
	  VoC_bgt16_rr(Coolsand_FilterUpdateCotrol_Int_FrameCnt_end,REG4,REG5)// if (pVarStepSize->d >= (0x1<<pVarStepSize->f) )
			 VoC_lw16i(REG5,0x7f);   //  pVarStepSize->d = (0x1<<pVarStepSize->f)-1;
Coolsand_FilterUpdateCotrol_Int_FrameCnt_end: 
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG5,REG0,DEFAULT);
	VoC_return;

} 

/**************************************************************************************
 * Function:    Coolsand_UpdateFilter
 *
 * Description: Update Adaptive Filter
 *
 * Inputs:         
 *	 REG7: ch
 *
 * Outputs: no
 *	 
 *    
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 1.1  optimized by  Cuilf     08/14/2010
 **************************************************************************************/
void Coolsand_UpdateFilterAmp(void)
{

	//Input : REG7  ch

	//
	// Compute Total Step Size
	//
	VoC_push16(RA,DEFAULT);


	VoC_lw16i(REG0,r_AF_SpkSigPower);
	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_lw32z(ACC0,IN_PARALLEL);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);//r_AF_SpkSigPower+ch
	VoC_lw16_d(ACC0_HI,m_AECProcPara_InitRegularFactor);
	VoC_add32_rp(REG45,ACC0,REG0,DEFAULT);  //	Fix tempSpkPower = *pSpkSigPower+RegularFactor;

	VoC_shr16_ri(REG5,-2,DEFAULT);
	VoC_lw32z(RL6,IN_PARALLEL);

	VoC_shr16_ri(REG5,2,DEFAULT);// 	Truncate(tempSpkPower, SpkSigPower_r.t, SpkSigPower_r.f);
	VoC_lw16i_short(REG2,14,IN_PARALLEL);

	//REG5 : SpkSigPower_r.d 

	VoC_bgtz16_r(Coolsand_ShrinkBit_abs,REG5)
		VoC_lw16i_short(REG3,0,DEFAULT);
		VoC_sub16_rr(REG5,REG3,REG5,DEFAULT);
Coolsand_ShrinkBit_abs:


	VoC_movreg16(RL6_LO,REG5,DEFAULT);

	VoC_jal(Coolsand_MSBPos);//xuml
	VoC_lw16i_short(REG4,0,DEFAULT);

	VoC_bez16_r(Coolsand_ShrinkBit_end,REG3)

	VoC_lw16i_short(REG4,1,DEFAULT);
	VoC_sub16_rr(REG3,REG4,REG3,DEFAULT);
	VoC_shr16_rr(REG4,REG3,DEFAULT);
	VoC_and16_rr(REG4,REG5,DEFAULT);   //  	SpkSigPower_r.d = ShrinkBit(SpkSigPower_r.d, SpkSigPower_r.t, 1);
	Coolsand_ShrinkBit_end:

	//input : RL6, REG2
	//output: REG3
	VoC_movreg16(RL6_LO,REG4,DEFAULT);
	//	 VoC_lw16i_short(REG2,7,DEFAULT);
	VoC_jal(Coolsand_MSBPos);              //int SpkPowerFirstBitPos = MSBPos( SpkSigPower_r.d,SpkSigPower_r.t-1);

	VoC_lw16i(REG1,g_AF_StepSize);
	VoC_lw16i(REG2,g_AF_ErrDataBufI);

	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_lw16i_short(REG4,7,IN_PARALLEL);

	VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);//	int StepSizeShiftVal = (SpkSigPower_r.f  - SpkPowerFirstBitPos+1-pVarStepSize->f);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);    //*pErrDataI

	VoC_lw16_p(REG1,REG1,DEFAULT);
	VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL); 

	VoC_shr16_rr(REG1,REG3,DEFAULT);  // 	TotalStepSize.d = pVarStepSize->d <<StepSizeShiftVal ;
	VoC_lw32z(ACC0,IN_PARALLEL);

	VoC_lw16i(REG3,g_AF_ErrDataBufQ);

	//
	// Compute Update Item
	//

	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	VoC_shr16_ri(REG1,-2,IN_PARALLEL);

	VoC_shr16_ri(REG1,2,DEFAULT);
	VoC_pop16(RA,IN_PARALLEL);

	VoC_multf32_rp(REG45,REG1,REG2,DEFAULT);//tempFixI = TotalStepSize*(*pErrDataI);
	VoC_shr16_ri(REG7,-3,IN_PARALLEL);

	VoC_msu32_rp(REG1,REG3,DEFAULT);// tempFixQ = TotalStepSize*(CongjErrDataQ);	
	VoC_movreg16(REG6,REG7,IN_PARALLEL);

	VoC_lw16i(REG0,r_AF_FilterAmpI);

	VoC_add16_rd(REG0,REG0,g_ch);	

	VoC_shr32_ri(REG45,10,DEFAULT);//Truncate(tempFixI,15+1+4, 15);
	VoC_shr32_ri(ACC0,10,IN_PARALLEL);//Truncate(tempFixQ,15+1+4, 15);

	VoC_shr32_ri(REG45,-1,DEFAULT);
	VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_movreg32(REG23,ACC0,IN_PARALLEL);//tempFixQ

	VoC_shr16_ri(REG0,5,DEFAULT);
	VoC_lw16i_short(REG1,1,IN_PARALLEL);

	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L1,REG0,REG1)
		VoC_lw16i(REG1,0x4000);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L1:

	VoC_lw16i_short(REG1,2,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L2,REG0,REG1)
		VoC_lw16i(REG1,0x2000);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L2:

	VoC_lw16i_short(REG1,3,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L3,REG0,REG1)
		VoC_lw16i(REG1,0x1555);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L3:

	VoC_lw16i_short(REG1,4,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L4,REG0,REG1)
		VoC_lw16i(REG1,0x1000);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L4:

	VoC_lw16i_short(REG1,5,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L5,REG0,REG1)
		VoC_lw16i(REG1,0xccc);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L5:

		VoC_lw16i_short(REG1,6,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L6,REG0,REG1)
		VoC_lw16i(REG1,0xaaa);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L6:

	VoC_lw16i_short(REG1,7,DEFAULT);
	VoC_bgt16_rr(Coolsand_UpdateFilterAmp_L7,REG0,REG1)
		VoC_lw16i(REG1,0x924);
	VoC_jump(Coolsand_UpdateFilterAmp_End);
Coolsand_UpdateFilterAmp_L7:
	VoC_lw16i(REG1,0x800);
Coolsand_UpdateFilterAmp_End:

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_shru16_ri(REG2,1,IN_PARALLEL);

	VoC_lw16i_short(FORMAT32,-1,DEFAULT); 
	VoC_lw16i_short(FORMATX,14,IN_PARALLEL); 

	VoC_multf32_rr(ACC0,REG3,REG1,DEFAULT);
	VoC_multf32_rr(ACC1,REG5,REG1,IN_PARALLEL);

	VoC_macX_rr(REG2,REG1,DEFAULT);
	VoC_macX_rr(REG4,REG1,IN_PARALLEL);

// VoC_directive: PARSER_OFF
	VoC_NOP();
	VoC_NOP();
	VoC_NOP();
	VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
{
	TempFixI = REGL[1];
	TempFixQ = REGL[0];
	fprintf(AFUpdateItem_fileid, "TempFix: %d + %di\n", REGL[0], REGL[1]);
}
#endif
// VoC_directive: PARSER_ON

	VoC_lw16i_set_inc(REG0,r_AF_SpkSigBufI,1);      
	VoC_lw16i_set_inc(REG1,r_AF_SpkSigBufQ,1);   

	VoC_movreg32(REG45,ACC1,DEFAULT);

	VoC_shr32_ri(REG45,-1,DEFAULT);
	VoC_shr32_ri(ACC0,-1,IN_PARALLEL);

	/*
	FilterUpdateI.f = Filter_FixFormat.f;
	FilterUpdateI.t = FilterUpdateI.f+1;
	FilterUpdateQ.t = FilterUpdateI.t;
	FilterUpdateQ.f = FilterUpdateI.f;

	for(i=0; i<FILTER_LEN; i++)
	{
	SpkSigI.d = pSpkSigBufI[i];
	SpkSigQ.d = pSpkSigBufQ[i];

	CmplxMul(&SpkSigI, &SpkSigQ, &tempFixI, &tempFixQ, &FilterUpdateI, &FilterUpdateQ);
	g_AF_UpdateItemI[i] = FilterUpdateI.d;
	g_AF_UpdateItemQ[i] = FilterUpdateQ.d;
	}
	*/


	VoC_lw16i_set_inc(REG2,r_AF_FilterBufI,2);
	VoC_lw16i_set_inc(REG3,r_AF_FilterBufQ,2);

	VoC_add32_rr(REG01,REG01,REG67,DEFAULT);
	VoC_add32_rr(REG23,REG23,REG67,IN_PARALLEL);

	VoC_add32_rr(REG23,REG23,REG67,DEFAULT);
	VoC_lw16i_short(FORMAT32,-4,IN_PARALLEL); 

	VoC_movreg32(REG67,ACC0,DEFAULT);
	VoC_lw16i_short(FORMATX,11,IN_PARALLEL); 


	//REG45 :tempFixI
	//RL7 : tempFixQ
	
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_AF_DEBUG
{
	int i = 0;
	short *pSpkSigBufI = (short *)&RAM_X[(REGS[0].reg-RAM_X_BEGIN_ADDR)/2];
	short *pSpkSigBufQ = (short *)&RAM_X[(REGS[1].reg-RAM_X_BEGIN_ADDR)/2];
	
	for (i = 0; i < 8; i++)
	{
		fprintf(AFUpdateItem_fileid, "pSpkSigBuf: %d + %di\t", pSpkSigBufQ[i], pSpkSigBufI[i]);
	}
	fprintf(AFUpdateItem_fileid, "\n");
}
#endif
// VoC_directive: PARSER_ON

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_shru16_ri(REG6,1,IN_PARALLEL);

#ifndef AEC_VOC_AF_DEBUG
	// TODO: This is still some error in this loop
	VoC_loop_i_short(8,DEFAULT) //	for(i=0; i<FILTER_LEN; i++)
	VoC_startloop0

		/*
		 * REG0: pointer to pSpkSigBufI
		 * REG1: pointer to pSpkSigBufQ
		 * REG45: TempFixI
		 * REG67: TempFixQ
		 */

		VoC_multf32_rp(ACC0,REG5,REG0,DEFAULT);
		VoC_multf32_rp(ACC1,REG7,REG0,IN_PARALLEL);

		VoC_macX_rp(REG4,REG0,DEFAULT);
		VoC_macXinc_rp(REG6,REG0,IN_PARALLEL);

		VoC_NOP();
		VoC_NOP();
		VoC_NOP();
		VoC_NOP();
// VoC_directive: PARSER_OFF
		if (REGL[0] > 524287)
		{
			REGL[0] = 524287;
			OverflowPrint(3163, "Coolsand_UpdateFilterAmp");
		}
		else if (REGL[0] < -524288)
		{
			REGL[0] = -524288;
			OverflowPrint(3168, "Coolsand_UpdateFilterAmp");
		}
// VoC_directive: PARSER_ON
		VoC_NOP();
		VoC_NOP();

		VoC_msu32_rp(REG7,REG1,DEFAULT);
		VoC_mac32_rp(REG5,REG1,IN_PARALLEL);

		VoC_msuX_rp(REG6,REG1,DEFAULT); 
		VoC_macXinc_rp(REG4,REG1,IN_PARALLEL);

		VoC_NOP();

		VoC_shr32_ri(ACC0,-12,DEFAULT);//	Truncate(tempFixI, pDstI->t, pDstI->f);
		VoC_shr32_ri(ACC1,-12,IN_PARALLEL); //	Truncate(tempFixQ, pDstI->t, pDstI->f);

		VoC_shr32_ri(ACC0,12,DEFAULT);//	Truncate(tempFixI, pDstI->t, pDstI->f);
		VoC_shr32_ri(ACC1,12,IN_PARALLEL); //	Truncate(tempFixQ, pDstI->t, pDstI->f);

		VoC_NOP();

// VoC_directive: PARSER_OFF
		VoC_NOP();
		VoC_NOP();
#ifdef AEC_VOC_AF_DEBUG
{
	fprintf(AFUpdateItem_fileid, "AFUpdateItem: %d + %di\t", REGL[1], REGL[0]);
}
#endif
// VoC_directive: PARSER_ON

		VoC_add32_rp(RL6,ACC0,REG2,DEFAULT); // 	AddVec(pFilterBufI, (int*)g_AF_UpdateItemI, pFilterBufI, FILTER_LEN);
		VoC_add32_rp(RL7,ACC1,REG3,DEFAULT);  // 	AddVec(pFilterBufQ, (int*)g_AF_UpdateItemQ, pFilterBufQ, FILTER_LEN);

		VoC_sw32inc_p(RL6,REG2,DEFAULT);
		VoC_sw32inc_p(RL7,REG3,DEFAULT);

	VoC_endloop0
#else
// VoC_directive: PARSER_OFF
{
	int i = 0;
	short *pSpkSigBufI = (short *)&RAM_X[(REGS[0].reg-RAM_X_BEGIN_ADDR)/2];
	short *pSpkSigBufQ = (short *)&RAM_X[(REGS[1].reg-RAM_X_BEGIN_ADDR)/2];
	int *pFilterBufI = (int *)&RAM_X[(REGS[2].reg-RAM_X_BEGIN_ADDR)/2];
	int *pFilterBufQ = (int *)&RAM_X[(REGS[3].reg-RAM_X_BEGIN_ADDR)/2];

#define Q19_MAX (524287)
#define Q19_MIN (-524288)
	
	for (i = 0; i < 8; i++)
	{
		int aI = pSpkSigBufI[i], aQ = pSpkSigBufQ[i];
		int bI = TempFixI, bQ = TempFixQ;
		int updateItemI, updateItemQ;
		int tempI, tempQ;

		//aI = -2650;
		//aQ = -3130;
		//bI = -110789;
		//bQ = -112490;

		tempI = (aI * bI) >> 11;
		tempQ = (aQ * bQ) >> 11;

		if (tempI > Q19_MAX)
		{
			tempI = Q19_MAX;
			OverflowPrint(3163, "Coolsand_UpdateFilterAmp");
		}
		else if (tempI < Q19_MIN)
		{
			tempI = Q19_MIN;
			OverflowPrint(3168, "Coolsand_UpdateFilterAmp");
		}

		if (tempQ > Q19_MAX)
		{
			tempQ = Q19_MAX;
			OverflowPrint(3174, "Coolsand_UpdateFilterAmp");
		}
		else if (tempQ < Q19_MIN)
		{
			tempQ = Q19_MIN;
			OverflowPrint(3179, "Coolsand_UpdateFilterAmp");
		}

		updateItemI = tempI - tempQ;
		if (updateItemI > Q19_MAX)
		{
			updateItemI = Q19_MAX;
			OverflowPrint(3186, "Coolsand_UpdateFilterAmp");
		}
		else if (updateItemI < Q19_MIN)
		{
			updateItemI = Q19_MIN;
			OverflowPrint(3191, "Coolsand_UpdateFilterAmp");
		}
		
		pFilterBufI[i] += updateItemI;

		tempI = (aI * bQ) >> 11;
		tempQ = (aQ * bI) >> 11;

		if (tempI > Q19_MAX)
		{
			tempI = Q19_MAX;
			OverflowPrint(3202, "Coolsand_UpdateFilterAmp");
		}
		else if (tempI < Q19_MIN)
		{
			tempI = Q19_MIN;
			OverflowPrint(3207, "Coolsand_UpdateFilterAmp");
		}

		if (tempQ > Q19_MAX)
		{
			tempQ = Q19_MAX;
			OverflowPrint(3213, "Coolsand_UpdateFilterAmp");
		}
		else if (tempQ < Q19_MIN)
		{
			tempQ = Q19_MIN;
			OverflowPrint(3218, "Coolsand_UpdateFilterAmp");
		}

		updateItemQ = tempI + tempQ;
		if (updateItemQ > Q19_MAX)
		{
			updateItemQ = Q19_MAX;
			OverflowPrint(3225, "Coolsand_UpdateFilterAmp");
		}
		else if (updateItemQ < Q19_MIN)
		{
			updateItemQ = Q19_MIN;
			OverflowPrint(3230, "Coolsand_UpdateFilterAmp");
		}

		pFilterBufQ[i] += updateItemQ;

		fprintf(AFUpdateItem_fileid, "AFUpdateItem: %d + %di\t", updateItemQ, updateItemI);
	}
#undef Q19_MAX
#undef Q19_MIN
}
// VoC_directive: PARSER_ON
#endif

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_AF_DEBUG
{
	fprintf(AFUpdateItem_fileid, "\n");
	fflush(AFUpdateItem_fileid);
}
#endif
// VoC_directive: PARSER_ON

	//
	// Filter Update
	//
 

	 VoC_return

}




/**************************************************************************************
 * Function:    Coolsand_BackCopyVec
 *
 * Description: Copy Src to Dst from the last index to zero
 *
 * Inputs:       
 *		REG1: (pSrc)pointer to Source Data [In]
 *		REG2: (pDst)pointer to Destination Data [Out]
 *		REG7: (Len)Data Length [In].shorts
 *
 * Outputs:     no
 *    
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 *
 * NOTE:the len must be even and shorts.
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_BackCopyVec(void)
{
	VoC_lw16i_short(INC1,-2,DEFAULT);
	VoC_lw16i_short(INC2,-2,IN_PARALLEL);

	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

	VoC_shr16_ri(REG7,1,DEFAULT);

	VoC_inc_p(REG1,DEFAULT);
	VoC_inc_p(REG2,IN_PARALLEL);

	VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    
	VoC_loop_r_short(REG7,DEFAULT)
	VoC_startloop0
		VoC_lw32inc_p(ACC0,REG1,DEFAULT);
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
	VoC_endloop0
		
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_FlipVec
 *
 * Description: Flip Src to Dst 
 *
 * Inputs:       
 *		REG1: (pSrc)pointer to Source Data [In]
 *		REG2: (pDst)pointer to Destination Data [Out]
 *		REG7: (Len)Data Length [In].shorts
 *
 * Outputs:     no
 *    
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 *
 * NOTE:
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_FlipVec(void)
{
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);	
	VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);		
    
	VoC_loop_r_short(REG7,DEFAULT);
	VoC_inc_p(REG2,IN_PARALLEL);	
	VoC_startloop0
		VoC_lw16inc_p(ACC0_HI,REG1,DEFAULT);
		VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
	VoC_endloop0
		
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_MulVec
 *
 * Description: the multiplication of vectors. //Mul Filter Coef.
 *
 * Inputs:       
 *		REG1: pTapppedDelayLine [in]
 *		REG2: AnaFilterCoef [in]
 *		REG3: g_FilterBank_MulBuf [out]
 *		REG0: filter Length [In]
 *
 * Outputs:     no
 *    
 * Used : REG0,REG1,REG2,REG3,REG6,REG7,INC1,INC2,INC3
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_MulVec(void)
{

	VoC_lw16i_short(INC1,2,DEFAULT);
	VoC_lw16i_short(INC2,1,IN_PARALLEL);	

	VoC_lw16i_short(INC3,2,DEFAULT);
	VoC_shr16_ri(REG0,1,IN_PARALLEL);
    
	VoC_loop_r_short(REG0,DEFAULT)
	VoC_lw32inc_p(REG67,REG1,IN_PARALLEL);	
	VoC_startloop0
		VoC_multf16inc_rp(REG6,REG6,REG2,DEFAULT);		
		VoC_multf16inc_rp(REG7,REG7,REG2,DEFAULT);
		VoC_NOP();
		VoC_lw32inc_p(REG67,REG1,DEFAULT);
		VoC_sw32inc_p(REG67,REG3,DEFAULT);
	VoC_endloop0
		
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_ClearVec
 *
 * Description: Clear vector. 
 *
 * Inputs:       
 *		
 *		REG1: pBuf: pointer to Data [In/Out]
 *		REG7:  Len: Data Length [In]
 *
 * Outputs:     no
 *    
 * Used : REG1,REG7,ACC0,INC1
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/24/2010
 **************************************************************************************/
void Coolsand_ClearVec(void)
{
	VoC_lw16i_short(INC1,2,DEFAULT);

	VoC_lw32z(ACC0,DEFAULT);
	VoC_shr16_ri(REG7,1,IN_PARALLEL);
    
	VoC_loop_r_short(REG7,DEFAULT)	
	VoC_startloop0
		VoC_sw32inc_p(ACC0,REG1,DEFAULT);
	VoC_endloop0
		
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_AddVec
 *
 * Description: the additation of vectors. 
 *
 * Inputs:       
 *		REG0: g_FilterBank_FoldSumBuf
 *		REG1: g_FilterBank_MulBuf+i*SUBBAND_CHANNEL_NUM
 *		REG2: g_FilterBank_FoldSumBuf
 *		REG7: filter Length [In]
 *
 * Outputs:     no
 *    
 * Used : REG0,REG1,REG2,REG6,REG7,INC0,INC1,INC2
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/24/2010
 **************************************************************************************/
void Coolsand_AddVec(void)
{
	
	VoC_loop_r_short(REG7,DEFAULT)
	VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
	VoC_startloop0
		VoC_add16inc_rp(REG6,REG6,REG1,DEFAULT);
		VoC_lw16inc_p(REG6,REG0,DEFAULT);
		VoC_sw16inc_p(REG6,REG2,DEFAULT);		
	VoC_endloop0
		
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_FFT16
 *
 * Description:  FFT N=16. 
 *
 * Inputs:       
 *		REG0: real 
 *		REG1: imag
 *		
 *
 * Outputs:     no
 *    
 * Used : all
 *
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/27/2010
 **************************************************************************************/
void Coolsand_IFFT16(void)
{

	VoC_push16(RA,DEFAULT);

	VoC_lw16i_short(INC0,2,DEFAULT);
	VoC_push32(REG01,IN_PARALLEL);

	//reverse real
	VoC_jal(Coolsand_ReverseIndex);

	VoC_pop32(REG23,DEFAULT);

	VoC_movreg16(REG0,REG3,DEFAULT);
	VoC_lw16i_short(INC0,2,IN_PARALLEL);

	VoC_push32(REG23,DEFAULT);
	//reverse imag
	VoC_jal(Coolsand_ReverseIndex);
	
	VoC_pop32(REG01,DEFAULT);
	VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

	VoC_lw16i_set_inc(REG2,real_int,2);
	VoC_lw16i_set_inc(REG3,imag_int,2);

	VoC_lw16i_short(INC1,2,DEFAULT);
	VoC_lw16i_short(INC0,2,IN_PARALLEL);	

	VoC_lw16i(REG6,0x7fff);

	VoC_loop_i_short(8,DEFAULT)
	VoC_push32(REG01,IN_PARALLEL);
	VoC_startloop0

		VoC_lw32inc_p(REG45,REG0,DEFAULT);//real[u],real[l]		
		
		VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u],imag[l]
		VoC_multf32_rr(ACC0,REG5,REG6,IN_PARALLEL);

		VoC_movreg16(REG7,RL7_HI,DEFAULT);
		VoC_movreg16(RL7_HI,RL7_LO,IN_PARALLEL);

		VoC_multf32_rr(RL6,REG7,REG6,DEFAULT);
		VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL);

		VoC_shr32_ri(ACC1,16,DEFAULT);
		VoC_shr32_ri(RL7,16,IN_PARALLEL);

		VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);//real[l]=real[u]-XkWnreal;
		VoC_add32_rr(ACC0,ACC1,ACC0,IN_PARALLEL);//real[u]=real[u]+XkWnreal;

		VoC_sub32_rr(RL7,RL7,RL6,DEFAULT);//imag[l]=imag[u]-XkWnimag;
		VoC_add32_rr(RL6,RL7,RL6,IN_PARALLEL);//imag[u]=imag[u]+XkWnimag;
		
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
		VoC_sw32inc_p(ACC1,REG2,DEFAULT);
		VoC_sw32inc_p(RL6,REG3,DEFAULT);
		VoC_sw32inc_p(RL7,REG3,DEFAULT);

	VoC_endloop0


	VoC_lw16i_set_inc(REG0,real_int,4);
	VoC_lw16i_set_inc(REG1,imag_int,4);

	VoC_loop_i_short(4,DEFAULT)
	VoC_lw16i_short(FORMATX,0,IN_PARALLEL);
	VoC_startloop0

		VoC_push32(REG01,DEFAULT);

		VoC_lw32inc_p(RL6,REG0,DEFAULT);//real[u]
		VoC_lw16i_short(REG2,-1,IN_PARALLEL);

		VoC_lw32_p(REG45,REG0,DEFAULT); //real[l]
		VoC_shru16_ri(REG2,1,IN_PARALLEL); //for getting 0x7fff

		VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u]
		VoC_shr32_ri(REG45,-1,IN_PARALLEL);

		VoC_lw32_p(REG67,REG1,DEFAULT); //imag[l]
		VoC_shru16_ri(REG4,1,IN_PARALLEL);

		VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
		VoC_shr32_ri(REG67,-1,IN_PARALLEL);
	
		VoC_macX_rr(REG5,REG2,DEFAULT);
		VoC_shru16_ri(REG6,1,IN_PARALLEL);

		VoC_multf32_rr(ACC1,REG6,REG2,DEFAULT);
		
		VoC_pop32(REG01,DEFAULT);
		VoC_macX_rr(REG7,REG2,IN_PARALLEL);

		VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
		VoC_add32_rr(RL6,RL6,ACC0,IN_PARALLEL);

		VoC_sub32_rr(ACC1,RL7,ACC1,DEFAULT);
		VoC_add32_rr(RL7,RL7,ACC1,IN_PARALLEL);

		VoC_sw32inc_p(RL6,REG0,DEFAULT);
		VoC_sw32inc_p(ACC0,REG0,DEFAULT);
		VoC_sw32inc_p(RL7,REG1,DEFAULT);
		VoC_sw32inc_p(ACC1,REG1,DEFAULT);

	VoC_endloop0


	VoC_lw16i_set_inc(REG0,real_int+2,4);
	VoC_lw16i_set_inc(REG1,imag_int+2,4);

	VoC_loop_i_short(4,DEFAULT)
	VoC_lw16i_short(FORMATX,0,IN_PARALLEL);
	VoC_startloop0

		VoC_push32(REG01,DEFAULT);

		VoC_lw32inc_p(RL6,REG0,DEFAULT);//real[u]
		VoC_lw16i_short(REG2,-1,IN_PARALLEL);

		VoC_lw32_p(REG45,REG0,DEFAULT); //real[l]
		VoC_shru16_ri(REG2,1,IN_PARALLEL); //for getting 0x7fff

		VoC_lw32inc_p(RL7,REG1,DEFAULT);//imag[u]
		VoC_shr32_ri(REG45,-1,IN_PARALLEL);

		VoC_lw32_p(REG67,REG1,DEFAULT); //imag[l]
		VoC_shru16_ri(REG4,1,IN_PARALLEL);

		VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
		VoC_shr32_ri(REG67,-1,IN_PARALLEL);
	
		VoC_macX_rr(REG5,REG2,DEFAULT);
		VoC_shru16_ri(REG6,1,IN_PARALLEL);

		VoC_multf32_rr(ACC1,REG6,REG2,DEFAULT);
		
		VoC_pop32(REG01,DEFAULT);
		VoC_macX_rr(REG7,REG2,IN_PARALLEL);

		VoC_sub32_rr(ACC0,RL7,ACC0,DEFAULT);
		VoC_add32_rr(RL7,RL7,ACC0,IN_PARALLEL);

		VoC_add32_rr(ACC1,RL6,ACC1,DEFAULT);
		VoC_sub32_rr(RL6,RL6,ACC1,IN_PARALLEL);

		VoC_sw32inc_p(RL7,REG1,DEFAULT);
		VoC_sw32inc_p(ACC0,REG1,DEFAULT);

		VoC_sw32inc_p(RL6,REG0,DEFAULT);
		VoC_sw32inc_p(ACC1,REG0,DEFAULT);
		

	VoC_endloop0



	VoC_lw16i_set_inc(REG0,real_int,16);
	VoC_lw16i_set_inc(REG1,imag_int,16);


	//load the address of real and imag 
	VoC_lw16i_set_inc(REG2,wnreal,2);
	VoC_lw16i_set_inc(REG3,wnimag,2);

	VoC_loop_i(1,4)

		VoC_lw16inc_p(REG4,REG2,DEFAULT);
		VoC_lw16i_short(REG7,8,IN_PARALLEL);

		VoC_lw16inc_p(REG5,REG3,DEFAULT);

		VoC_push32(REG01,DEFAULT)

		VoC_add16_rr(REG6,REG7,REG0,DEFAULT);
		VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);

		VoC_loop_i_short(2,DEFAULT)
		VoC_push32(REG23,IN_PARALLEL);
		VoC_startloop0
			
			VoC_lw32_p(REG23,REG6,DEFAULT);

			VoC_lw32_p(REG67,REG7,DEFAULT);
			VoC_shr32_ri(REG23,-1,IN_PARALLEL);

			VoC_push32(REG67,DEFAULT);
			VoC_shr32_ri(REG67,-1,IN_PARALLEL);

			VoC_shru16_ri(REG2,1,DEFAULT);
			VoC_shru16_ri(REG6,1,IN_PARALLEL);

			VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
			VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

			VoC_macX_rr(REG4,REG3,DEFAULT);
			VoC_macX_rr(REG5,REG3,IN_PARALLEL);

			VoC_msu32_rr(REG5,REG6,DEFAULT);
			VoC_mac32_rr(REG4,REG6,IN_PARALLEL);

			VoC_msuX_rr(REG5,REG7,DEFAULT);
			VoC_macX_rr(REG4,REG7,IN_PARALLEL);

			VoC_pop32(REG67,DEFAULT);

			//real[l]=real[u]-XkWnreal;
			//real[u]=real[u]+XkWnreal;
			VoC_sub32_pr(RL6,REG0,ACC0,DEFAULT);
			VoC_add32_rp(ACC0,ACC0,REG0,IN_PARALLEL);

			//imag[l]=imag[u]-XkWnimag;
			//imag[u]=imag[u]+XkWnimag;
			VoC_sub32_pr(RL7,REG1,ACC1,DEFAULT);
			VoC_add32_rp(ACC1,ACC1,REG1,IN_PARALLEL);

			VoC_sw32_p(RL6,REG6,DEFAULT);
			VoC_lw16i_short(REG2,8,IN_PARALLEL);					
			
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);			
			VoC_add16_rr(REG6,REG2,REG0,IN_PARALLEL);

			VoC_sw32inc_p(ACC1,REG1,DEFAULT);
			VoC_add16_rr(REG7,REG2,REG1,IN_PARALLEL);

			VoC_sw32_p(RL7,REG7,DEFAULT);

		VoC_endloop0
		
		VoC_pop32(REG23,DEFAULT);		

		VoC_pop32(REG01,DEFAULT);
		VoC_lw16i_short(REG5,2,IN_PARALLEL);

		VoC_add16_rr(REG0,REG5,REG0,DEFAULT);
		VoC_add16_rr(REG1,REG5,REG1,IN_PARALLEL);		
		
	VoC_endloop1


	VoC_lw16i_set_inc(REG0,real_int,2);
	VoC_lw16i_set_inc(REG1,imag_int,2);


	//load the address of real and imag 
	VoC_lw16i_set_inc(REG2,wnreal,1);
	VoC_lw16i_set_inc(REG3,wnimag,1);

	VoC_lw16i_short(REG7,16,DEFAULT);

	VoC_add16_rr(REG6,REG7,REG0,DEFAULT);
	VoC_add16_rr(REG7,REG7,REG1,IN_PARALLEL);
	
	VoC_loop_i_short(8,DEFAULT)
	VoC_startloop0

		VoC_lw16inc_p(REG4,REG2,DEFAULT);
		VoC_lw16inc_p(REG5,REG3,DEFAULT);

		VoC_push32(REG23,DEFAULT);		
			
			VoC_lw32_p(REG23,REG6,DEFAULT);

			VoC_lw32_p(REG67,REG7,DEFAULT);
			VoC_shr32_ri(REG23,-1,IN_PARALLEL);

			VoC_push32(REG67,DEFAULT);
			VoC_shr32_ri(REG67,-1,IN_PARALLEL);

			VoC_shru16_ri(REG2,1,DEFAULT);
			VoC_shru16_ri(REG6,1,IN_PARALLEL);

			VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
			VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

			VoC_macX_rr(REG4,REG3,DEFAULT);
			VoC_macX_rr(REG5,REG3,IN_PARALLEL);

			VoC_msu32_rr(REG5,REG6,DEFAULT);
			VoC_mac32_rr(REG4,REG6,IN_PARALLEL);

			VoC_msuX_rr(REG5,REG7,DEFAULT);
			VoC_macX_rr(REG4,REG7,IN_PARALLEL);

			VoC_pop32(REG67,DEFAULT);

			//real[l]=real[u]-XkWnreal;
			//real[u]=real[u]+XkWnreal;
			VoC_sub32_pr(RL6,REG0,ACC0,DEFAULT);
			VoC_add32_rp(ACC0,ACC0,REG0,IN_PARALLEL);

			//imag[l]=imag[u]-XkWnimag;
			//imag[u]=imag[u]+XkWnimag;
			VoC_sub32_pr(RL7,REG1,ACC1,DEFAULT);
			VoC_add32_rp(ACC1,ACC1,REG1,IN_PARALLEL);

			VoC_sw32_p(RL6,REG6,DEFAULT);
			VoC_lw16i_short(REG4,16,IN_PARALLEL);					
			
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);			
			VoC_add16_rr(REG6,REG4,REG0,IN_PARALLEL);

			VoC_sw32inc_p(ACC1,REG1,DEFAULT);

			VoC_pop32(REG23,DEFAULT);		
			
			VoC_sw32_p(RL7,REG7,DEFAULT);
			VoC_add16_rr(REG7,REG4,REG1,IN_PARALLEL);

		VoC_endloop0

	VoC_pop32(REG23,DEFAULT);		

	VoC_lw16i_set_inc(REG0,real_int,2);
	VoC_lw16i_set_inc(REG1,imag_int,2);
	
	VoC_lw16i_short(INC2,1,DEFAULT);
	VoC_lw16i_short(INC3,1,IN_PARALLEL)


	VoC_loop_i(1,2)

		VoC_lw32inc_p(ACC0,REG0,DEFAULT);

		VoC_lw32inc_p(ACC1,REG0,DEFAULT);
		VoC_movreg32(RL6,ACC0,IN_PARALLEL);


		VoC_loop_i_short(8,DEFAULT);
		VoC_movreg32(RL7,ACC1,IN_PARALLEL);	
		VoC_startloop0

			VoC_lw16i_short(STATUS,OVF_CLR,DEFAULT);
			VoC_shr32_ri(RL6,31,DEFAULT);

			VoC_xor32_rr(ACC0,RL6,DEFAULT);
			VoC_shr32_ri(RL7,31,IN_PARALLEL);
			
			VoC_xor32_rr(ACC1,RL7,DEFAULT);
			VoC_sub32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

			
			VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);
			VoC_shr32_ri(ACC0,4,IN_PARALLEL);
							
			VoC_xor32_rr(ACC0,RL6,DEFAULT);
			VoC_shr32_ri(ACC1,4,IN_PARALLEL);
			
			VoC_xor32_rr(ACC1,RL7,DEFAULT);
			
			VoC_boe0z16_r(Coolsand_IFFT16_L0)
				VoC_lw32z(RL7,DEFAULT);
Coolsand_IFFT16_L0:

			VoC_boe1z16_r(Coolsand_IFFT16_L1)
				VoC_lw32z(RL6,DEFAULT);
Coolsand_IFFT16_L1:

			VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);
			VoC_sub32_rr(ACC0,ACC0,RL6,IN_PARALLEL);

			VoC_shr32_ri(ACC0,-16,DEFAULT);	
			VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

			VoC_lw32inc_p(ACC0,REG0,DEFAULT);

			VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
			VoC_movreg32(RL6,ACC0,IN_PARALLEL);

			VoC_lw32inc_p(ACC1,REG0,DEFAULT);

			VoC_sw16inc_p(ACC1_HI,REG2,DEFAULT);
			VoC_movreg32(RL7,ACC1,IN_PARALLEL);		

		VoC_endloop0
			
		VoC_movreg16(REG0,REG1,DEFAULT);
		VoC_movreg16(REG2,REG3,DEFAULT);

	VoC_endloop1


	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_ReverseIndex
 *
 * Description: reverse the index when fft. 
 *
 * Inputs:       
 *		REG0: real or imag
 *
 * Outputs:     no
 *
 * Used : all
 * 
 * NOTE:   no
 *
 * Version 1.0  Created by  Xuml       07/26/2010
 **************************************************************************************/
void Coolsand_ReverseIndex(void)
{
	

	VoC_push16(REG0,DEFAULT);

	//load data
	VoC_lw32inc_p(RL7,REG0,DEFAULT);
	VoC_lw32inc_p(RL6,REG0,DEFAULT);
	VoC_lw32inc_p(ACC1,REG0,DEFAULT);
	VoC_lw32inc_p(ACC0,REG0,DEFAULT);
	VoC_lw32inc_p(REG67,REG0,DEFAULT);
	VoC_lw32inc_p(REG45,REG0,DEFAULT);
	VoC_lw32inc_p(REG23,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG0,DEFAULT);

	VoC_pop16(REG0,DEFAULT);

	//reverse the index
	VoC_movreg16(REG6,RL7_HI,DEFAULT);
	VoC_movreg16(RL7_HI,REG6,IN_PARALLEL);
	
	VoC_movreg16(ACC1_LO,RL6_LO,DEFAULT);
	VoC_movreg16(RL6_LO,ACC1_LO,IN_PARALLEL);

	VoC_movreg16(REG2,RL6_HI,DEFAULT);
	VoC_movreg16(RL6_HI,REG2,IN_PARALLEL);

	VoC_movreg16(REG4,ACC1_HI,DEFAULT);
	VoC_movreg16(ACC1_HI,REG4,IN_PARALLEL);

	VoC_movreg16(REG1,ACC0_HI,DEFAULT);
	VoC_movreg16(ACC0_HI,REG1,IN_PARALLEL);

	VoC_movreg16(REG3,REG5,DEFAULT);
	VoC_movreg16(REG5,REG3,IN_PARALLEL);
	
	//save data
	VoC_sw32inc_p(RL7,REG0,DEFAULT);
	VoC_sw32inc_p(RL6,REG0,DEFAULT);
	VoC_sw32inc_p(ACC1,REG0,DEFAULT);
	VoC_sw32inc_p(ACC0,REG0,DEFAULT);
	VoC_sw32inc_p(REG67,REG0,DEFAULT);
	VoC_sw32inc_p(REG45,REG0,DEFAULT);
	VoC_sw32inc_p(REG23,REG0,DEFAULT);
	VoC_sw16_p(REG1,REG0,DEFAULT);
	
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_CopyVec
 *
 * Description: Copy Src to Dst
 *
 * Inputs:       
 *		REG1: (pSrc)pointer to Source Data [In]
 *		REG2: (pDst)pointer to Destination Data [Out]
 *		REG7: (Len)Data Length [In].shorts
 *
 * Outputs: no
 * 
 * Used : REG1,REG2,REG7,ACC0,INC1,INC2
 * 
 * NOTE:the len must be even and shorts.
 * 
 * Version 1.0  Created by  Xuml       07/23/2010
 **************************************************************************************/
void Coolsand_CopyVec(void)
{

	VoC_lw32inc_p(ACC0,REG1,DEFAULT);
	VoC_shr16_ri(REG7,1,IN_PARALLEL);
    
	VoC_loop_r_short(REG7,DEFAULT)
	VoC_startloop0
		VoC_lw32inc_p(ACC0,REG1,DEFAULT);
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
	VoC_endloop0
		
    VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_CmplxAppxNorm_int
 *
 * Description: no
 *
 * Inputs:       
 *   ACC0: pSrcI   
 *   ACC1: pSrcQ
 *
 * Outputs: ACC0.
 *    
 * Used : REG45,ACC0,ACC1
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 2.0  Created by  Xuml       08/25/2010
 **************************************************************************************/
void Coolsand_CmplxAppxNorm_int(void)
{
	
	VoC_movreg32(REG45,ACC0,DEFAULT);
	VoC_shr32_ri(ACC0,31,IN_PARALLEL);

	VoC_xor32_rr(REG45,ACC0,DEFAULT);
	VoC_push32(RL6,IN_PARALLEL);
	
	VoC_movreg32(RL6,ACC1,DEFAULT);
	VoC_shr32_ri(ACC1,31,IN_PARALLEL);

	VoC_xor32_rr(RL6,ACC1,DEFAULT);
	VoC_sub32_rr(ACC0,REG45,ACC0,IN_PARALLEL);

	VoC_sub32_rr(ACC1,RL6,ACC1,DEFAULT);

   VoC_bgt32_rr(Coolsand_CmplxAppxNorm_int_300,ACC0,ACC1)
     VoC_movreg32(ACC0,ACC1,DEFAULT);
     VoC_movreg32(ACC1,ACC0,IN_PARALLEL);//swap
Coolsand_CmplxAppxNorm_int_300:


    VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_movreg32(REG45,ACC1,IN_PARALLEL);
	
	VoC_shr32_ri(ACC0,-5,DEFAULT);//MaxVal*32
	VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

 	VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);//MaxVal*31 == MaxVal*32 - MaxVal 
	VoC_add32_rr(ACC1,ACC1,REG45,IN_PARALLEL);//MinVal*3 ==MinVal*2+MinVal

    VoC_shr32_ri(ACC0,5,DEFAULT); //(MaxVal*31)>>5
	VoC_shr32_ri(ACC1,3,IN_PARALLEL);
        
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
	VoC_pop32(RL6,IN_PARALLEL);

	VoC_return;

  

}


/**************************************************************************************
 * Function:    Coolsand_MSBPos
 *
 * Description: no
 *
 * Inputs:  RL6:Data 
 *			REG2:BitNum
 *	 
 *
 * Outputs: REG3:return;
 *	 
 *    
 * Used : REG2,REG3,ACC0,RL6,INC3.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Cuilf       08/05/2010
 * Version 2.0  Created by  Xuml        08/25/2010
 **************************************************************************************/
void Coolsand_MSBPos(void)
{

	VoC_movreg32(ACC0,RL6,DEFAULT);
	VoC_shr32_ri(RL6,31,IN_PARALLEL);

	VoC_xor32_rr(ACC0,RL6,DEFAULT);
	VoC_push32(ACC0,IN_PARALLEL);

	VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);
	VoC_lw16i_short(REG3,0,IN_PARALLEL);

	VoC_loop_r_short(REG2,DEFAULT);
	VoC_lw16i_short(INC3,1,IN_PARALLEL);
	VoC_startloop0
		VoC_bez32_r(Coolsand_MSBPos_L0,ACC0)
		VoC_inc_p(REG3,DEFAULT);
		VoC_shr32_ri(ACC0,1,IN_PARALLEL);
	VoC_endloop0

Coolsand_MSBPos_L0:
	VoC_pop32(ACC0,DEFAULT);

	VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_NonLinearTransform
 *
 * Description: no
 *
 * Inputs:       
 *   REG0:Spk
 *	 
 *
 * Outputs: 
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/20/2010
 **************************************************************************************/

/*
void Coolsand_NonLinearTransform(void)
{
	VoC_lw16i_short(REG6,-15,DEFAULT);
	VoC_lw16i_short(REG7,-15,IN_PARALLEL);

	VoC_sub16_rd(REG7,REG7,NonLinear_Beta_order_ADDR);

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw32z(ACC1,IN_PARALLEL);

	VoC_lw16i_short(ACC0_LO,1,DEFAULT);
	VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);

	VoC_shr32_rr(ACC0,REG6,DEFAULT);
	VoC_shr32_rr(ACC1,REG7,IN_PARALLEL);

	VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
	VoC_sub32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);

	VoC_shr32_ri(ACC0,1,DEFAULT);
	VoC_shr32_ri(ACC1,1,IN_PARALLEL);

	VoC_shr32_ri(ACC0,-16,DEFAULT);
	VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

	VoC_shr32_ri(ACC0,16,DEFAULT);
	VoC_shr32_ri(ACC1,16,IN_PARALLEL);

	VoC_lw32z(RL6,DEFAULT);

	VoC_sw32_d(ACC0,UpLimit);
	VoC_sw32_d(ACC1,DnLimit);
	
	VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
	VoC_sub32_rr(ACC1,RL6,ACC1,IN_PARALLEL);


//	VoC_lw16i(REG6,NonLinearTransform_T_LO);
//	VoC_lw16i(REG7,NonLinearTransform_T_HI);

	VoC_lw32_d(REG67,NonLinear_T_ADDR)

	VoC_sw32_d(ACC0,Minus_UpLimit);
	VoC_sw32_d(ACC1,Minus_DnLimit);

	
	VoC_lw16i_short(FORMATX,0,DEFAULT);
	VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

	VoC_shr32_ri(REG67,-1,DEFAULT);
	VoC_lw16i_short(INC0,1,IN_PARALLEL);

	VoC_shru16_ri(REG6,1,DEFAULT);

	VoC_lw16_d(REG1,SoftLimiter_Beta_order_ADDR);
		
	VoC_loop_i(0,DEF_DATA_BLOCK_SIZE)

		VoC_multf32_rp(ACC0,REG6,REG0,DEFAULT);
		VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

		VoC_macX_rp(REG7,REG0,DEFAULT);
		VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);

		VoC_lw16i(REG4,cos_tab);
		
		VoC_bnlt32_rd(Coolsand_NonLinearTransform_L0,ACC0,Minus_UpLimit)

			VoC_lw16i_short(REG2,0,DEFAULT);
			VoC_sub16_rd(REG2,REG2,NonLinear_Scale_ADDR);
			VoC_movreg16(ACC0_HI,REG2,DEFAULT);
			//VoC_lw16i(ACC0_HI,-NonLinearTransform_Scale);
			VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L0:


		VoC_bgt32_rd(Coolsand_NonLinearTransform_L1,ACC0,Minus_DnLimit)
			

			VoC_shr32_ri(ACC1,-14,DEFAULT);
			VoC_lw16i_short(REG5,32,IN_PARALLEL);
			
			VoC_add32_rr(REG23,ACC0,ACC1,DEFAULT);
			VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);			

			VoC_shr32_rr(REG23,REG1,DEFAULT);
			VoC_lw32z(ACC1,IN_PARALLEL);

			VoC_sub32_rr(REG23,ACC1,REG23,DEFAULT);

			VoC_shr32_ri(REG23,15-NonLinearTransform_CosPhaseBitNum,DEFAULT);

			
			VoC_add16_rr(REG4,REG4,REG2,DEFAULT);			
			VoC_lw16i_short(REG4,0,DEFAULT);

			VoC_lw16_p(REG5,REG4,DEFAULT);
			VoC_sub16_rr(REG4,REG4,REG1,IN_PARALLEL);

			VoC_shr16_rr(REG5,REG4,DEFAULT);
			
			VoC_shr32_ri(REG45,16,DEFAULT);
			VoC_sub32_rd(ACC0,ACC0,DnLimit);
			
			VoC_sub32_rr(REG45,ACC0,REG45,DEFAULT);
			VoC_shr32_ri(REG45,-1,DEFAULT);
			VoC_shru16_ri(REG4,1,DEFAULT);
			VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

			VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
			VoC_macX_rr(REG5,REG2,DEFAULT);
			VoC_NOP();
			VoC_shr32_ri(ACC0,-16,DEFAULT);
			
		

			VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L1:
			
		VoC_bnlt32_rd(Coolsand_NonLinearTransform_L2,ACC0,DnLimit)

			VoC_movreg32(REG45,ACC0,DEFAULT);
			VoC_shr32_ri(REG45,-1,DEFAULT);
			VoC_lw16i_short(FORMATX,-1,IN_PARALLEL);

			VoC_shru16_ri(REG4,1,DEFAULT);
			VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);
			VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

			VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
			VoC_macX_rr(REG5,REG2,DEFAULT);

			VoC_lw16i_short(FORMATX,0,DEFAULT);
			VoC_lw16i_short(FORMAT32,15,IN_PARALLEL);

			VoC_shr32_ri(ACC0,-16,DEFAULT);			

			VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L2:


		VoC_bgt32_rd(Coolsand_NonLinearTransform_L3,ACC0,UpLimit)
			

			VoC_shr32_ri(ACC1,-14,DEFAULT);
			VoC_lw16i_short(REG5,32,IN_PARALLEL);
			
			VoC_sub32_rr(REG23,ACC0,ACC1,DEFAULT);
			VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);			

			VoC_shr32_rr(REG23,REG1,DEFAULT);
			VoC_shr32_ri(REG23,15-NonLinearTransform_CosPhaseBitNum,DEFAULT);

			
			VoC_add16_rr(REG4,REG4,REG2,DEFAULT);			
			VoC_lw16i_short(REG4,0,DEFAULT);

			VoC_lw16_p(REG5,REG4,DEFAULT);
			VoC_sub16_rr(REG4,REG4,REG1,IN_PARALLEL);

			VoC_shr16_rr(REG5,REG4,DEFAULT);

			VoC_shr32_ri(REG45,16,DEFAULT);
			VoC_add32_rd(ACC0,ACC0,DnLimit);
			
			VoC_add32_rr(REG45,ACC0,REG45,DEFAULT);
			VoC_shr32_ri(REG45,-1,DEFAULT);
			VoC_shru16_ri(REG4,1,DEFAULT);
			VoC_lw16_d(REG2,NonLinear_Scale_ADDR);

			VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
			VoC_macX_rr(REG5,REG2,DEFAULT);
			VoC_NOP();
			VoC_shr32_ri(ACC0,-16,DEFAULT);
			VoC_jump(Coolsand_NonLinearTransform_End);
Coolsand_NonLinearTransform_L3:

		VoC_lw16_d(ACC0_HI,NonLinear_Scale_ADDR);
		
		VoC_NOP();		
Coolsand_NonLinearTransform_End:
		VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);

		
	VoC_endloop0

	VoC_return;

	


}

/**************************************************************************************
 * Function:    Coolsand_ComputeNoiseWeight_DeciDirectCTRL
 *
 * Description:  
 *
 * Inputs:       
 *   REG7:
 *	 
 *
 * Outputs: 
 *	 
 *    
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 * Version 2.0  Created by  Guosz      11/12/2015
 **************************************************************************************/
void Coolsand_ComputeNoiseWeight_DeciDirectCTRL(void)
{
	VoC_push16(RA,DEFAULT);
	
	// Compute Instant Err Power
	VoC_lw16i(REG0,g_AF_ErrDataBufI);
	VoC_lw16i(REG1,g_AF_ErrDataBufQ);
	VoC_lw16i(REG2,r_PF_SmoothErrPower);
	VoC_lw16_d(REG4,m_AF_FrameCnt);
	VoC_lw16_d(REG7,g_ch);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

	VoC_lw16_p(ACC0_HI,REG0,DEFAULT);
	VoC_lw16_p(ACC1_HI,REG1,DEFAULT);
	VoC_shr32_ri(ACC0,16,DEFAULT);
	VoC_shr32_ri(ACC1,16,IN_PARALLEL);
	//InstantErrNorm.f = pErrDataI->f;
	//InstantErrNorm.t = pErrDataI->f+1;
	//CmplxAppxNorm(pErrDataI, pErrDataQ, &InstantErrNorm);
	VoC_jal(Coolsand_CmplxAppxNorm_int);	// return ACC0
    VoC_bngt32_rd(DeciDirectCTRL_not_saturation_L00,ACC0,AEC_CONST_0x00007fff)
        VoC_lw32_d(ACC0,AEC_CONST_0x00007fff);
DeciDirectCTRL_not_saturation_L00:
    VoC_bgt32_rd(DeciDirectCTRL_not_saturation_L01,ACC0,AEC_CONST_0xffff8000)
        VoC_lw32_d(ACC0,AEC_CONST_0xffff8000);
DeciDirectCTRL_not_saturation_L01:
    VoC_NOP();
	VoC_movreg32(REG45,ACC0,DEFAULT);
	VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);
	VoC_NOP();
	//InstantErrPower = InstantErrNorm*InstantErrNorm;
	//Truncate(InstantErrPower, 32, 31);
	VoC_multf32_rr(RL7,REG4,REG4,DEFAULT);
//	VoC_shr32_ri(REG45,-8,DEFAULT);
	VoC_lw16i_short(REG1,4,DEFAULT);
	VoC_NOP();
	VoC_push32(RL7,DEFAULT);
//		VoC_shr32_ri(RL7,-4,DEFAULT);
	VoC_lw32_p(ACC0,REG2,DEFAULT);
//	if (InstantErrPower > *pSmoothErrPower)
// Modify a bug, old VERSION is 	if (InstantErrNorm > *pSmoothErrPower)
	VoC_bgt32_rr(DeciDirectCTRL_L12,RL7,ACC0)
		VoC_lw16i_short(REG1,1,DEFAULT);
DeciDirectCTRL_L12:
	//*pSmoothErrPower = FirstOrderIIRSmooth2(pSmoothErrPower, &InstantErrPower, &ErrSmoothFactor); 
//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);	// ACC0 for *pSmoothErrPower
		// extern VoC_jal(Coolsand_FirstOrderIIRSmooth2);
	//Truncate(NewValue, pOldValue->f-pUpdateFactor->f+1, pOldValue->f-pUpdateFactor->f);
//	*pSmoothErrPower = FirstOrderIIRSmooth2(pSmoothErrPower, &InstantErrPower, &ErrSmoothFactor); 
		VoC_lw16i_short(FORMAT32,-11,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
        VoC_lw16i_short(FORMAT32,4,DEFAULT);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		//VoC_shr32_ri(RL7,-15,DEFAULT);
        VoC_NOP();
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
		//VoC_shr32_ri(REG45,4,DEFAULT);
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);	//	NewValue = NewValue+(*pOldValue);
		VoC_NOP();
		VoC_sw32_p(REG45,REG2,DEFAULT);

	VoC_lw16i(REG0,r_PF_NoisePowerEstBuf);
	VoC_lw16i(REG1,g_PF_pSmoothErrPowerMin1);
	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_lw16i_set_inc(REG2,g_PF_pSmoothErrPowerMin2,2);
	VoC_lw16i_set_inc(REG3,r_PF_NoiseFloorDevCnt,2);
	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);
//	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

	VoC_lw32_p(RL6,REG0,DEFAULT);	//*pNoisePowerEst
	VoC_lw32_p(RL7,REG1,DEFAULT);	//pSmoothErrPowerMin1
//	if (*pNoisePowerEst > *pSmoothErrPower)
//		*pNoisePowerEst = *pSmoothErrPower;
	VoC_bngt32_rr(DeciDirectCTRL_L13,RL6,REG45)
		VoC_movreg32(RL6,REG45,DEFAULT);
DeciDirectCTRL_L13:

//	if (*pSmoothErrPowerMin1 > TempNoisePower)
//		*pSmoothErrPowerMin1 = TempNoisePower;
	VoC_bngt32_rr(DeciDirectCTRL_L14,RL7,REG45)
		VoC_movreg32(RL7,REG45,DEFAULT);
DeciDirectCTRL_L14:
	VoC_sw32_p(RL6,REG0,DEFAULT);	//*pNoisePowerEst
	VoC_sw32_p(RL7,REG1,DEFAULT);	//pSmoothErrPowerMin1
	VoC_movreg32(ACC0,REG45,IN_PARALLEL);
	VoC_bez32_r(DeciDirectCTRL_L15,ACC0)
		VoC_lw16_d(REG6,m_AF_FrameCnt);
//		VoC_lw16i(REG5,600);	//		NoiseFloorCntLimit = 400;
		VoC_lw16_d(REG5,(m_AECProcPara_NoiseCntLimitNum+3));
		VoC_lw16_d(REG4,(m_AECProcPara_FrameCntLimitNum+2));
		VoC_bngt16_rr(DeciDirectCTRL_L16,REG4,REG6)	//	else if (FrameCnt < 5000)
//			VoC_lw16i(REG5,400);	//		NoiseFloorCntLimit = 300;
			VoC_lw16_d(REG5,(m_AECProcPara_NoiseCntLimitNum+2));
DeciDirectCTRL_L16:
		VoC_lw16_d(REG4,(m_AECProcPara_FrameCntLimitNum+1));
		VoC_bngt16_rr(DeciDirectCTRL_L17,REG4,REG6)	//	else if (FrameCnt < 2000)
//			VoC_lw16i(REG5,300);	//		NoiseFloorCntLimit = 200;
			VoC_lw16_d(REG5,(m_AECProcPara_NoiseCntLimitNum+1));
DeciDirectCTRL_L17:
		VoC_lw16_d(REG4,m_AECProcPara_FrameCntLimitNum);
		VoC_bngt16_rr(DeciDirectCTRL_L18,REG4,REG6)	//	else if (FrameCnt < 500)
//			VoC_lw16i(REG5,200);	//		NoiseFloorCntLimit = 100;
			VoC_lw16_d(REG5,m_AECProcPara_NoiseCntLimitNum);
DeciDirectCTRL_L18:
	VoC_lw16i_short(REG6,1,DEFAULT);
	VoC_add16_rp(REG6,REG6,REG3,DEFAULT);	//	*pNoiseFloorDevCnt = (*pNoiseFloorDevCnt) + 1;
	VoC_bngt16_rr(DeciDirectCTRL_L115,REG6,REG5)	//	if (*pNoiseFloorDevCnt > NoiseFloorCntLimit)
		VoC_lw16i_short(REG6,0,DEFAULT);	//		*pNoiseFloorDevCnt = 0;
		VoC_lw32_p(ACC1,REG2,DEFAULT);	//		Fix Temp = *pSmoothErrPowerMin2;
		VoC_bngt32_rr(DeciDirectCTRL_L20,ACC1,ACC0)	//		if (Temp > TempNoisePower)
		VoC_movreg32(ACC1,ACC0,DEFAULT);	//			Temp = TempNoisePower;

DeciDirectCTRL_L20:
		VoC_bngt32_rr(DeciDirectCTRL_L21,ACC1,RL7)		//		if (Temp > *pSmoothErrPowerMin1)
		VoC_movreg32(ACC1,RL7,DEFAULT);			//	Temp = *pSmoothErrPowerMin1;

DeciDirectCTRL_L21:
		VoC_sw32_p(ACC0,REG1,DEFAULT);	//		*pSmoothErrPowerMin1 = TempNoisePower;
		VoC_sw32_p(ACC1,REG0,DEFAULT);	//		*pNoisePowerEst = Temp;
		VoC_sw32_p(RL7,REG2,DEFAULT);	//		*pSmoothErrPowerMin2 = *pSmoothErrPowerMin1;
DeciDirectCTRL_L115:
		VoC_sw16_p(REG6,REG3,DEFAULT);	// save pNoiseFloorDevCnt
DeciDirectCTRL_L15:
//	VoC_push32(REG23,DEFAULT);
//	BiasCorrectFactor.f = 0;
//	BiasCorrectFactor.t = BiasCorrectFactor.f+1+2;
//	BiasCorrectFactor = Double2Fix(2.0, BiasCorrectFactor.t, BiasCorrectFactor.f);

    //no use BiasCorrectFactor
	//VoC_lw16i_short(REG4,2,DEFAULT);
	//VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
//	NoisePowerEst_r = *pNoisePowerEst;
//	Truncate(NoisePowerEst_r, pNoisePowerEst->t-4, pNoisePowerEst->f-4);

    // we do not need to truncate NoisePowerEst_r to (pNoisePowerEst->t-4, pNoisePowerEst->f-4)
	VoC_lw32_p(REG23,REG0,DEFAULT);
	//VoC_shr32_ri(REG23,4,DEFAULT);
//	NoisePowerEst_r = NoisePowerEst_r*BiasCorrectFactor;
//	Truncate(NoisePowerEst_r, pNoisePowerEst->t-4, pNoisePowerEst->f-4);
	//VoC_multf32_rr(REG23,REG2,REG4,DEFAULT);
	VoC_NOP();
	VoC_bnez32_r(DeciDirectCTRL_L22,REG23)//	if (NoisePowerEst_r.d==0)
		VoC_lw16i_short(REG2,1,DEFAULT);//		NoisePowerEst_r.d = 1; 
DeciDirectCTRL_L22:
	//VoC_push32(REG23,DEFAULT);//	NoisePowerEst_r2 = NoisePowerEst_r;

	//VoC_lw16i_short(REG4,0x1c,DEFAULT);
    VoC_lw16i(REG4,0x20);
	VoC_push32(ACC0,DEFAULT);	// SAVE pSmoothErrPower
	VoC_jal(Coolsand_ShrinkBit);	//	NoisePowerEst_r.d = ShrinkBit(NoisePowerEst_r.d, NoisePowerEst_r.t, 1);
//	NoisePowerEstRev = 1.0/Fix2Double(NoisePowerEst_r);
	VoC_pop32(RL6,DEFAULT);	// load pSmoothErrPower
	VoC_sw32_d(ACC0,Temp_gsz);	// SAVE NoisePowerEst_r.d

	//VoC_pop32(RL7,DEFAULT);//	NoisePowerEst_r2
    VoC_movreg32(RL7,ACC0,DEFAULT);// NoisePowerEst_r
//	VoC_shr32_ri(ACC0,-7,DEFAULT);
	// Update Post SNR
//	*pPostSToNRatio = Double2Fix( Fix2Double(InstantErrPower)*NoisePowerEstRev, pPostSToNRatio->t, pPostSToNRatio->f );
	VoC_pop32(REG23,DEFAULT); //InstantErrPower

    //Todo: Just Use Low 16 bit of ACC0,it maybe overflow
	VoC_movreg16(REG4,ACC0_LO,DEFAULT);
	VoC_lw16i_short(REG5,2,IN_PARALLEL);
	//VoC_shr32_ri(REG23,-3,DEFAULT);
    VoC_shr32_ri(REG23,-7,DEFAULT);
DeciDirectCTRL_L124:
	VoC_bgt16_rr(DeciDirectCTRL_L123,REG5,REG4)
		VoC_shr16_ri(REG4,1,DEFAULT);
		VoC_shr32_ri(REG23,1,IN_PARALLEL);
		VoC_jump(DeciDirectCTRL_L124);
DeciDirectCTRL_L123:
	//	VoC_multf32_rr(REG45,REG4,REG2,DEFAULT);
//	PostSigPower = *pSmoothErrPower - (NoisePowerEst_r2);
	VoC_lw16i(ACC1_LO,0X7FFF);
	VoC_lw16i_short(ACC1_HI,0,DEFAULT);
	VoC_bgt32_rr(DeciDirectCTRL_L223,ACC1,REG23)
		VoC_movreg32(REG23,ACC1,DEFAULT);
DeciDirectCTRL_L223:
	VoC_lw16i(REG6,g_PF_PostSToNRatio);
	//VoC_shr32_ri(RL7,-4,DEFAULT);
	//VoC_add16_rr(REG6,REG6,REG7,IN_PARALLEL);
	VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    //PostSigPower = *pSmoothErrPower - NoisePowerEst_r;
	VoC_sub32_rr(ACC1,RL6,RL7,DEFAULT);
	VoC_sw16_p(REG2,REG6,DEFAULT);	//	save pPostSToNRatio
	//VoC_lw16i_short(REG6,1,IN_PARALLEL);

    //new code here 2016/10/14
	//VoC_be16_rr(DeciDirectCTRL_L23,REG6,REG7)//	if (ch != 1)
	//VoC_bnlt32_rd(DeciDirectCTRL_L23,RL6,g_PF_SmoothErrPowerSub1)//		if (*pSmoothErrPowerSub1 > *pSmoothErrPower)
		//VoC_sub32_dr(ACC1,g_PF_SmoothErrPowerSub1,RL7);		//		PostSigPower = *pSmoothErrPowerSub1 - NoisePowerEst_r2;
//DeciDirectCTRL_L23:
    //Fix temp = *pSmoothErrPowerSub1;
    VoC_lw32_d(REG45,g_PF_SmoothErrPowerSub1);

    VoC_bez16_r(DeciDirectCTRL_L23_0,REG7)
    VoC_lw16_d(REG6,m_AECProcPara_NoiseWeightSub1DecayFactor);

    //for (i = 0; i < ch; i++)
    //{
        //temp.d = temp.d >> 4; // NoiseWeightSub1DecayFactor is Q4
        //temp.d = temp.d * NoiseWeightSub1DecayFactor;
	//}
    VoC_loop_r(0,REG7)
        VoC_shr32_ri(REG45,4,DEFAULT);
        VoC_lw16i_short(FORMATX,0,IN_PARALLEL);
        VoC_shr32_ri(REG45,-1,DEFAULT);
        VoC_lw16i_short(FORMAT32,-15,IN_PARALLEL);
        VoC_multf32_rr(ACC0,REG5,REG6,DEFAULT);
        VoC_shru16_ri(REG4,1,IN_PARALLEL);
        VoC_macX_rr(REG4,REG6,DEFAULT);
        VoC_NOP();
        VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_endloop0
    //RL7: NoisePowerEst_r
    //if (temp > *pSmoothErrPower)
    //{
        //PostSigPower = temp - NoisePowerEst_r;
	//}
    VoC_NOP();
DeciDirectCTRL_L23_0:
    VoC_bngt32_rr(DeciDirectCTRL_L23_1,REG45,RL6)
        VoC_sub32_rr(ACC1,REG45,RL7,DEFAULT);
DeciDirectCTRL_L23_1:

	VoC_bnltz32_r(DeciDirectCTRL_L24,ACC1)	//	if (PostSigPower.d < 0)
		VoC_lw32z(ACC1,DEFAULT);//		PostSigPower.d = 0;
DeciDirectCTRL_L24:
	VoC_lw16i(REG2,r_PF_PrevSigPowerNBuf);	
	VoC_movreg32(RL7,ACC1,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_lw16_d(REG1,m_AECProcPara_SNRUpdateFactor);
//	VoC_shr32_ri(RL7,4,DEFAULT);//	Truncate(PostSigPower, pPrevSigPowerN->t, pPrevSigPowerN->f);
	VoC_lw32_p(ACC0,REG2,DEFAULT);
	//  	PriorSigPower = FirstOrderIIRSmooth2(pPrevSigPowerN, &PostSigPower, &SNRUpdateFactor);
//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);	// ACC0 for *PriorSigPower
//		VoC_shr32_ri(RL7,-4,DEFAULT);
		VoC_lw16i_short(FORMAT32,-8,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL6,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
        VoC_lw16i_short(FORMAT32,7,DEFAULT);
		VoC_multf32_rr(RL7,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		//VoC_shr32_ri(RL6,-8,DEFAULT);
		//VoC_shr32_ri(RL7,7,DEFAULT);
        VoC_NOP();
		VoC_add32_rr(RL7,RL7,RL6,DEFAULT);
		//VoC_NOP();
		VoC_add32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = NewValue+(*pOldValue);
		//VoC_NOP();
//		VoC_sw32_p(REG45,REG2,DEFAULT);
	//	NoisePowerFirstBitPos = MSBPos( NoisePowerEst_r.d,NoisePowerEst_r.t-1);
	VoC_lw32_d(RL6,Temp_gsz);
	//VoC_lw16i_short(REG2,0x1b,DEFAULT);
    VoC_lw16i_short(REG2,0x1f,DEFAULT);
	//REG3:NoisePowerFirstBitPos
	VoC_jal(Coolsand_MSBPos);
	//VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG2,8,DEFAULT);
	VoC_sub16_rr(REG2,REG3,REG2,DEFAULT);	//	int PriorSNRShiftVal = NoisePowerEst_r.f+1-NoisePowerFirstBitPos-PriorSigPower.f+pPriorSToNRatio->f;
//	if (PriorSNRShiftVal >=0)
//		pPriorSToNRatio->d = PriorSigPower.d << PriorSNRShiftVal;
//	else
//		pPriorSToNRatio->d = PriorSigPower.d >> (-PriorSNRShiftVal);
	VoC_lw16i(REG3,g_PF_PriorSToNRatio);
	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	//VoC_lw16i(REG1,g_PF_NoiseWeight);
//	VoC_lw16_p(REG5,REG3,DEFAULT);
	VoC_shr32_rr(REG45,REG2,DEFAULT);
//	if (pPriorSToNRatio->d >= 0x1<<(pPriorSToNRatio->t-1))
//		pPriorSToNRatio->d = (0x1<<(pPriorSToNRatio->t-1))-1;
	VoC_lw16i(RL6_LO,0x7fff);
	VoC_lw16i_short(RL6_HI,0,DEFAULT);
	VoC_bgt32_rr(DeciDirectCTRL_L125,RL6,REG45)
		VoC_movreg32(REG45,RL6,DEFAULT);
DeciDirectCTRL_L125:
    VoC_bnltz32_r(DeciDirectCTRL_L126,REG45)
        VoC_movreg32(REG45,RL6,DEFAULT);
DeciDirectCTRL_L126:

	//VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16i(RL6_LO,127);
	VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);
	VoC_push16(REG4,DEFAULT);
//	NoiseWeight_r = *pNoiseWeight;
	//VoC_lw16_p(REG2,REG1,DEFAULT);
	//VoC_lw16i_short(REG3,0,IN_PARALLEL);
	//VoC_lw16i_short(REG4,8,DEFAULT);
	//VoC_jal(Coolsand_ShrinkBit);	//	NoiseWeight_r.d = ShrinkBit(NoiseWeight_r.d, NoiseWeight_r.t, 1);	???
//	NoiseWeight_r.d = NoiseWeight_r.d*2;
	//VoC_shr32_ri(ACC0,-1,DEFAULT);
	//VoC_lw16i(REG2,0X80);
	//VoC_movreg16(REG3,ACC0_LO,DEFAULT);
	//VoC_pop16(REG5,DEFAULT);
	//VoC_shr32_ri(REG45,16,DEFAULT);
	//VoC_bngt16_rr(DeciDirectCTRL_L25,REG2,REG3)		//	if (NoiseWeight_r.d >= (0x1<<(NoiseWeight_r.t-1)))
		//VoC_lw16i_short(FORMAT32,7,DEFAULT);
		//VoC_NOP();
		//VoC_multf32_rr(REG45,REG4,REG3,DEFAULT);	//		Mul_SNRWeight = *pPriorSToNRatio;
//DeciDirectCTRL_L25:
	//	else
//		Mul_SNRWeight = *pPriorSToNRatio*(NoiseWeight_r);
	//VoC_bnez16_r(DeciDirectCTRL_L26,REG4)		//		if (Mul_SNRWeight.d == 0)
		//VoC_lw16i_short(REG4,1,DEFAULT);	//	Mul_SNRWeight.d = 1;
//DeciDirectCTRL_L26:
    //Mul_SNRWeight = *pPriorSToNRatio + Double2Fix(0.99, pPriorSToNRatio->t, pPriorSToNRatio->f);
    VoC_add32_rr(RL6,RL6,REG45,DEFAULT);

//	int MulSNRWeightFirstBitPos = MSBPos( Mul_SNRWeight.d,Mul_SNRWeight.t-1);
	//VoC_movreg16(RL6_HI,REG4,DEFAULT);
	//VoC_shr32_ri(RL6,16,DEFAULT);
	//VoC_lw16i_short(REG2,0xf,DEFAULT);
    VoC_lw16i_short(REG2,0x10,DEFAULT);
	VoC_jal(Coolsand_MSBPos);
//	int NoiseWeightShiftVal = Mul_SNRWeight.f+1-MulSNRWeightFirstBitPos+(pNoiseWeight->f-NoiseOverEstFactor.f);
    VoC_lw16i_short(REG2,0xf,DEFAULT);
	VoC_sub16_rr(REG2,REG3,REG2,DEFAULT);
	VoC_lw16_d(REG4,m_AECProcPara_NoiseOverEstFactor);
//	if (NoiseWeightShiftVal>=0)
//		TempFix.d = NoiseOverEstFactor.d << NoiseWeightShiftVal;
//	else
//		TempFix.d = NoiseOverEstFactor.d >> (-NoiseWeightShiftVal);
//	Truncate(TempFix, TempFix.t, TempFix.f);
	VoC_shr16_rr(REG4,REG2,DEFAULT);

/*
	VoC_lw16i(REG5,0x2000);
	VoC_bgt16_rr(DeciDirectCTRL_L27,REG5,REG4)//	if (TempFix >= Double2Fix(64.0, 8, 0))
		VoC_lw16i(REG4,127);//		TempFix = Double2Fix(0.99, TempFix.t, TempFix.f);
		VoC_jump(DeciDirectCTRL_L31);
DeciDirectCTRL_L27:
	VoC_shr16_ri(REG5,1,DEFAULT);
	VoC_bgt16_rr(DeciDirectCTRL_L28,REG5,REG4)	//	else if(TempFix >= Double2Fix(32.0, 8, 0))
		VoC_lw16i(REG4,122);//		TempFix = Double2Fix(0.95, TempFix.t, TempFix.f);
		VoC_jump(DeciDirectCTRL_L31);
DeciDirectCTRL_L28:
	VoC_shr16_ri(REG5,3,DEFAULT);
	VoC_bgt16_rr(DeciDirectCTRL_L29,REG5,REG4)//	else if(TempFix >= Double2Fix(4.0, 8, 0))
		VoC_lw16i(REG4,115);//		TempFix = Double2Fix(0.9, TempFix.t, TempFix.f);
		VoC_jump(DeciDirectCTRL_L31);
DeciDirectCTRL_L29:
	VoC_shr16_ri(REG5,1,DEFAULT);
	VoC_bgt16_rr(DeciDirectCTRL_L30,REG5,REG4)//	else if (TempFix >= Double2Fix(2.0, 8, 0))
		VoC_lw16i(REG4,102);//		TempFix = Double2Fix(0.8, TempFix.t, TempFix.f);
		VoC_jump(DeciDirectCTRL_L31);
DeciDirectCTRL_L30:
//	VoC_shr16_ri(REG5,1,DEFAULT);
	VoC_lw16i(REG5,96);
	VoC_bgt16_rr(DeciDirectCTRL_L31,REG5,REG4)//	else if (TempFix >= Double2Fix(0.75, 8, 7))
		VoC_lw16i(REG4,96);//		TempFix = Double2Fix(0.75, TempFix.t, TempFix.f);
DeciDirectCTRL_L31:
*/
	VoC_lw16i(REG1,g_PF_NoiseWeight);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_lw16i(REG6,0x7f);
    //if (TempFix.d >= 0x1<<(pNoiseWeight->f))
    VoC_bngt16_rr(DeciDirectCTRL_L31,REG4,REG6)
        VoC_movreg16(REG4,REG6,DEFAULT);
DeciDirectCTRL_L31:

//	VoC_lw16i(REG5,m_AECProcPara_NoiseGainLimit);
    VoC_pop16(REG5,DEFAULT);//pPriorSToNRatio
    //pNoiseWeight->d = ((0x1<<(pNoiseWeight->f))-1) - TempFix.d;
	VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
//	*pNoiseWeight = FixMax(*pNoiseWeight, NoiseGainLimit);
//	VoC_lw16_d(REG3,m_AECProcPara_NoiseGainLimit);
	//VoC_bgt16_rd(DeciDirectCTRL_L32,REG6,m_AECProcPara_NoiseGainLimit)
		//VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
//DeciDirectCTRL_L32:

    //if (*pPriorSToNRatio < m_PF_NoiseGainLimitAdj)
    VoC_bnlt16_rd(DeciDirectCTRL_L32_0,REG5,m_PF_NoiseGainLimitAdj)
        VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
DeciDirectCTRL_L32_0:
    //if (ch == 1){}
    VoC_bne16_rr(DeciDirectCTRL_L32_1,REG7,REG4)
        VoC_lw16_d(REG4,m_PF_NoiseGainLimitAdj);
        VoC_blt16_rd(DeciDirectCTRL_L32_2,REG5,m_AECProcPara_NoiseWeightSNRTh1)
            VoC_sub16_rd(REG4,REG4,m_AECProcPara_NoiseWeightSNRLimitStep);
DeciDirectCTRL_L32_2:
        VoC_bnlt16_rd(DeciDirectCTRL_L32_3,REG5,m_AECProcPara_NoiseWeightSNRTh2)
            VoC_add16_rd(REG4,REG4,m_AECProcPara_NoiseWeightSNRLimitStep);
DeciDirectCTRL_L32_3:
        VoC_bngt16_rd(DeciDirectCTRL_L32_4,REG4,m_AECProcPara_NoiseWeightSNRLimitMax)
            VoC_lw16_d(REG4,m_AECProcPara_NoiseWeightSNRLimitMax);
DeciDirectCTRL_L32_4:
        VoC_bgt16_rd(DeciDirectCTRL_L32_5,REG4,m_AECProcPara_NoiseWeightSNRLimitMin)
            VoC_lw16_d(REG4,m_AECProcPara_NoiseWeightSNRLimitMin);
DeciDirectCTRL_L32_5:
        VoC_NOP();
        VoC_sw16_d(REG4,m_PF_NoiseGainLimitAdj);
DeciDirectCTRL_L32_1:

    VoC_bgt16_rd(DeciDirectCTRL_FixMax,REG6,m_AECProcPara_NoiseGainLimit)
        VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
DeciDirectCTRL_FixMax:

	VoC_lw16_d(REG4,m_AF_FrameCnt);
	VoC_lw16i(REG5,50);
	VoC_bngt16_rr(DeciDirectCTRL_L33,REG5,REG4)	//	if (FrameCnt < 50)      // added by chengxuefeng
		//VoC_lw16i_short(REG6,0,DEFAULT);//		pNoiseWeight->d = 0;
        VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
DeciDirectCTRL_L33:
	VoC_pop16(RA,DEFAULT);
	VoC_sw16_p(REG6,REG1,DEFAULT);
	VoC_return
}

/**************************************************************************************
 * Function:    Coolsand_ComputeNoiseWeight_DeciDirectCTRL
 *
 * Description:  
 *
 * Inputs:       
 *   REG7:
 *	 
 *
 * Outputs: 
 *	 
 *    
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
/*
void Coolsand_ComputeNoiseWeight_DeciDirectCTRL_old(void)
{
	VoC_push16(RA,DEFAULT);
	
	// Compute Instant Err Power
	VoC_lw16i(REG0,g_AF_ErrDataBufI);
	VoC_lw16i(REG1,g_AF_ErrDataBufQ);

	VoC_lw16_d(REG7,g_ch);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG1,DEFAULT);
	
	VoC_jal(Cooland_InstantPower);

	VoC_lw16_d(REG5,g_ch);
	VoC_lw16i(REG6,r_PF_SmoothErrPower);
	VoC_lw16_d(REG7,m_AECProcPara_DataSmoothFactor);

	VoC_movreg32(RL7,ACC0,DEFAULT);
	VoC_shr16_ri(REG5,-1,IN_PARALLEL);

	VoC_add16_rr(REG6,REG6,REG5,DEFAULT);

	VoC_push16(REG5,DEFAULT);
	

	VoC_jal(Coolsand_SmoothPeakPower);

	VoC_pop16(REG7,DEFAULT);

	VoC_lw16i(REG0,r_PF_NoisePowerEstBuf);
	VoC_lw16i(REG1,r_PF_SmoothErrPower);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

	VoC_lw32_p(ACC0,REG0,DEFAULT);
	VoC_lw32_p(ACC1,REG1,DEFAULT);
	VoC_push32(REG01,DEFAULT);

	// Instant Err SNR
	VoC_jal(Coolsand_InstantSNR); 

	//REG3:InstantSToNRatio
	
	//Spk  Noise Power Est
	VoC_jal(Coolsand_OverallNoiseEstwState);

	VoC_pop32(REG01,DEFAULT);//pNoisePowerEst

	VoC_lw16i_short(REG4,20,DEFAULT);

	VoC_lw32_p(REG23,REG0,DEFAULT);

	VoC_shr32_ri(REG23,-1,DEFAULT);//	NoisePowerEst_r = (*pNoisePowerEst)*BiasCorrectFactor;

	VoC_shr32_ri(REG23,4,DEFAULT);

	VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L0,REG23)
		VoC_lw16i_short(REG2,1,DEFAULT); 
		VoC_lw16i_short(REG3,0,IN_PARALLEL);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L0:

	// Compute Noise Power Reverse
	VoC_jal(Coolsand_ShrinkBit);

	//ACC0:NoisePowerEst_r.d

	VoC_lw16i_short(REG4,5,DEFAULT);
	VoC_lw32_p(REG67,REG1,IN_PARALLEL);
	
	VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);

	// Update Post SNR
	VoC_shr32_rr(REG67,REG4,DEFAULT);
	VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);

	VoC_movreg32(RL6,REG67,DEFAULT);
	VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

	VoC_and32_rr(RL6,RL7,DEFAULT);

	VoC_bez32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L1,RL6)
		VoC_add32_rr(REG67,REG67,RL7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L1:

	VoC_shr32_ri(REG67,-15,DEFAULT);
	VoC_movreg32(RL7,ACC0,IN_PARALLEL);

	VoC_shr32_ri(RL7,-4,DEFAULT);
		
//	VoC_shr32_ri(REG67,-4,DEFAULT);
//	VoC_movreg16(REG5,REG6,IN_PARALLEL);

	VoC_lw16i(REG2,g_PF_PostSToNRatio);

	VoC_add16_rd(REG2,REG2,g_ch);

	VoC_sub32_pr(RL7,REG1,RL7,DEFAULT);

	VoC_sw16_p(REG7,REG2,DEFAULT);

	VoC_bnltz32_r(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L2,RL7)
		VoC_lw32z(RL7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L2:

	VoC_lw16_d(REG2,g_ch);

	VoC_shr16_ri(REG2,-1,DEFAULT);
	VoC_push32(ACC0,IN_PARALLEL);
	
	VoC_lw16i(REG3,r_PF_PrevSigPowerNBuf);

	VoC_add16_rr(REG3,REG3,REG2,DEFAULT);

	VoC_lw16_d(REG1,m_AECProcPara_SNRUpdateFactor);

	VoC_lw32_p(ACC1,REG3,DEFAULT);
	VoC_shr32_ri(RL7,4,IN_PARALLEL);
			
	// Update Prior Sig Power
	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

	VoC_shr32_ri(REG45,-8,DEFAULT);
	VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG45,7,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

	VoC_mac32_rr(REG4,REG1,DEFAULT);

	VoC_macX_rr(REG5,REG1,DEFAULT);	
	
	VoC_NOP();
	
	//ACC0:PriorSigPower

	VoC_pop32(RL6,DEFAULT);

	VoC_push32(ACC0,DEFAULT);
	VoC_lw16i_short(REG2,20,IN_PARALLEL);
	
	// Update Prior SNR
	//	NoisePowerFirstBitPos = MSBPos( NoisePowerEst_r.d,NoisePowerEst_r.t-1);
	VoC_jal(Coolsand_MSBPos);

	VoC_lw16i(REG1,g_PF_PriorSToNRatio);

	VoC_add16_rd(REG1,REG1,g_ch);

	VoC_pop32(ACC0,DEFAULT);
	VoC_lw16i_short(REG0,8,IN_PARALLEL);

	VoC_sub16_rr(REG0,REG3,REG0,DEFAULT);

	VoC_shr32_rr(ACC0,REG0,DEFAULT);

//	if ( pPriorSToNRatio->d  >= 0x1<<(pPriorSToNRatio->t-1) )
//		pPriorSToNRatio->d = (0x1<<(pPriorSToNRatio->t-1))-1;

	VoC_lw16i(REG0,g_PF_NoiseWeight);

	VoC_add16_rd(REG0,REG0,g_ch);

	VoC_shr32_ri(ACC0,-16,DEFAULT);

	VoC_lw16_p(REG3,REG0,DEFAULT);
	VoC_lw16i_short(REG4,8,IN_PARALLEL);

	VoC_shr32_ri(REG23,16,DEFAULT);
	VoC_sw16_p(ACC0_HI,REG1,IN_PARALLEL);

	VoC_push16(REG0,DEFAULT);//save pNoiseWeight

	VoC_push16(ACC0_HI,DEFAULT);//save pPriorSToNRatio->d

		
	//Noise Weight
	VoC_jal(Coolsand_ShrinkBit);

	VoC_pop16(REG0,DEFAULT);//load pPriorSToNRatio->d
	VoC_shr32_ri(ACC0,-24,IN_PARALLEL);

	VoC_shr32_ri(ACC0,23,DEFAULT);
	VoC_lw16i_short(REG2,1,IN_PARALLEL);

	VoC_movreg16(REG1,ACC0_LO,DEFAULT);
	VoC_shr16_ri(REG2,-7,IN_PARALLEL);

	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L3,REG2,REG1)

		//Mul_SNRWeight = *pPriorSToNRatio*(NoiseWeight_r);
		VoC_multf16_rr(REG0,REG0,REG1,DEFAULT);
		VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L3:

	VoC_pop16(REG1,DEFAULT)//load pNoiseWeight

	VoC_lw16i_short(REG2,15,DEFAULT);

	VoC_add16_rp(REG0,REG0,REG1,DEFAULT);

	VoC_movreg16(RL6_HI,REG0,DEFAULT);
	VoC_push16(REG1,IN_PARALLEL);//save pNoiseWeight

	VoC_shr32_ri(RL6,16,DEFAULT);

	VoC_jal(Coolsand_MSBPos);
	
	VoC_lw16i_short(REG0,15,DEFAULT);
	VoC_lw16i_short(REG2,1,IN_PARALLEL);

	VoC_sub16_rr(REG3,REG3,REG0,DEFAULT);
	VoC_shr16_ri(REG2,-7,IN_PARALLEL);//1.0

	VoC_lw16_d(REG1,m_AECProcPara_NoiseOverEstFactor);

	VoC_shr16_rr(REG1,REG3,DEFAULT);//TempFix.d
	VoC_pop16(REG4,IN_PARALLEL);//load pNoiseWeight

	VoC_lw16i_short(REG5,0,DEFAULT);
	//	if (Fix2Double(TempFix) >= 1.0 )
	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L4,REG2,REG1)
		VoC_sub16_rr(REG5,REG2,REG1,DEFAULT);	
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L4:
	
	VoC_lw16_d(REG0,m_AECProcPara_NoiseGainLimitStep);
	
	VoC_mult16_rd(REG0,REG0,g_ch);

	VoC_NOP();

	VoC_add16_rd(REG0,REG0,m_AECProcPara_NoiseGainLimit);

	VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L5,REG5,REG0)
		VoC_movreg16(REG5,REG0,DEFAULT);	
Coolsand_ComputeNoiseWeight_DeciDirectCTRL_L5:	

	VoC_pop16(RA,DEFAULT);

	VoC_sw16_p(REG5,REG4,DEFAULT);

	VoC_return;

}
*/

/**************************************************************************************
 * Function:    Coolsand_ComputeNoiseWeight_DeciDirect
 *
 * Description:  Compute Noise Weight Through Decision Direct Method
 *
 * Inputs:       
 *   REG7:
 *	 
 *
 * Outputs: 
 *	 
 *    
 * Used : all.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/

/*
void Coolsand_ComputeNoiseWeight_DeciDirect(void)
{
	VoC_push16(RA,DEFAULT);

	// Compute Instant Err Power
	VoC_lw16i(REG0,g_AF_ErrDataBufI);
	VoC_lw16i(REG1,g_AF_ErrDataBufQ);

	VoC_lw16_d(REG7,g_ch);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG1,DEFAULT);

	//REG0:pErrDataI;[in]
	//REG1:pErrDataQ;[in]
	//ACC0:InstantErrNorm[out]
	VoC_jal(Coolsand_CmplxAppxNorm);

	//InstantErrPower = InstantErrNorm*InstantErrNorm;
	VoC_shr32_ri(ACC0,-16,DEFAULT);
	VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);
	
	VoC_movreg16(REG0,ACC0_HI,DEFAULT);	
	VoC_lw16i_short(REG1,4,IN_PARALLEL);
	
	VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
	VoC_shr16_ri(REG7,-1,IN_PARALLEL);

	VoC_lw16i(REG2,r_PF_SmoothErrPower);

	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_movreg32(RL7,ACC0,IN_PARALLEL);

	VoC_lw16i(REG3,r_PF_NoisePowerEstBuf);

	VoC_lw32_p(ACC1,REG2,DEFAULT);
	VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

	VoC_push32(RL7,DEFAULT);

	//RL7:InstantErrPower[in]
	//REG1:ErrSmoothFactor[in]
	//ACC1:*pSmoothErrPower[in]

	//ACC0:*pSmoothErrPower[out]

	VoC_shr32_ri(RL7,-4,DEFAULT);

	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	// Instant SNR
	VoC_shr32_ri(ACC0,-8,DEFAULT);
	VoC_lw32_p(RL6,REG3,IN_PARALLEL);

	VoC_shr32_ri(ACC0,8,DEFAULT);
	VoC_shr32_ri(RL6,4,IN_PARALLEL);

	VoC_push32(RL6,DEFAULT);//backup NoisePowerEst_r = *pNoisePowerEst;
	VoC_push16(REG3,IN_PARALLEL);
	
	VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L0,RL6)
		VoC_lw16i_short(RL6_LO,1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L0:	

	//	*pSmoothErrPower = FirstOrderIIRSmooth2(pSmoothErrPower, &InstantErrPower, &ErrSmoothFactor); 


	VoC_sw32_p(ACC0,REG2,DEFAULT);

	VoC_lw16i_short(REG2,20,DEFAULT);//xuml
	VoC_lw16i_short(REG4,1,IN_PARALLEL);

	//RL6:NoisePowerEst_r.d.[in]
	//REG2:NoisePowerEst_r.t-1.[in]

	////REG3: NoisePowerFirstBitPos.[out]
	VoC_jal(Coolsand_MSBPos);

	VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
	VoC_movreg32(RL6,ACC0,IN_PARALLEL);
	
	VoC_shr32_rr(ACC0,REG3,DEFAULT);
	VoC_pop16(REG7,IN_PARALLEL);

	//if (InstantSToNRatio.d >= (0x1<<(InstantSToNRatio.t-1)))
	//InstantSToNRatio.d = (0x1<<(InstantSToNRatio.t-1))-1;
	VoC_shr32_ri(ACC0,-24,DEFAULT);
	VoC_movreg32(RL7,ACC1,IN_PARALLEL);	

	VoC_shr32_ri(ACC0,24,DEFAULT);
	VoC_pop32(ACC1,IN_PARALLEL);			

	VoC_lw16_d(REG6,m_AECProcPara_NoiseUpdateFactor);

	VoC_push16(ACC0_LO,DEFAULT);//InstantSToNRatio
	VoC_push32(RL6,IN_PARALLEL);

	VoC_push32(ACC1,DEFAULT);//PrevNoisePowerEst


	
	//RL6:pSmoothErrPower
	//RL7:PrevSmoothErrPower
	//ACC1:pNoisePowerEst
	//REG6:NoiseUpdateFactor
	//ACC0:InstantSToNRatio
	//REG7:&pNoisePowerEst
	
	VoC_jal(Coolsand_NoisePSDEst);

	VoC_pop32(ACC1,DEFAULT);//PrevNoisePowerEst

	VoC_pop32(RL6,DEFAULT);//*pSmoothErrPower
	VoC_pop16(REG0,IN_PARALLEL);//InstantSToNRatio

	VoC_bez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L1,RL6)

		VoC_lw16i(REG2,r_PF_NoiseFloorDevCnt);
		VoC_lw16i(REG3,r_PF_NoisePowerEstBuf);
		VoC_lw16_d(REG1,g_ch);

		VoC_add16_rr(REG2,REG2,REG1,DEFAULT);
		VoC_shr16_ri(REG1,-1,IN_PARALLEL);

		VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
		
		VoC_lw16i_short(REG1,0,DEFAULT);
		VoC_lw16i_short(REG3,8*4,IN_PARALLEL);

		VoC_lw32_p(ACC0,REG3,DEFAULT);

		VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L2,REG0,REG3)
		VoC_bne32_rr  (Coolsand_ComputeNoiseWeight_DeciDirect_L2,ACC1,ACC0)

			VoC_lw16i_short(REG4,1,DEFAULT);	
			VoC_add16_rp(REG1,REG4,REG2,DEFAULT);
			
Coolsand_ComputeNoiseWeight_DeciDirect_L2:
		VoC_lw16i(REG5,500);
		VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L3,REG1,REG5)
			//*pNoiseFloorDevCnt = 0;
			//pNoisePowerEst->d = pNoisePowerEst->d*2;
			VoC_lw16i_short(REG1,0,DEFAULT);
			VoC_shr32_ri(ACC1,-1,IN_PARALLEL);
			VoC_NOP();
			VoC_sw32_p(ACC1,REG7,DEFAULT);			
Coolsand_ComputeNoiseWeight_DeciDirect_L3:
			VoC_sw16_p(REG1,REG2,DEFAULT);

Coolsand_ComputeNoiseWeight_DeciDirect_L1:
		
	// Bias Noise Power

    //NoisePowerEst_r = *pNoisePowerEst;
	VoC_lw32_p(ACC1,REG7,DEFAULT);

	//Truncate(NoisePowerEst_r, pNoisePowerEst->t-4, pNoisePowerEst->f-4);
	VoC_shr32_ri(ACC1,4,DEFAULT);

	//NoisePowerEst_r = NoisePowerEst_r*BiasCorrectFactor;
	VoC_shr32_ri(ACC1,-1,DEFAULT);
	VoC_shr32_ri(ACC1,-12,DEFAULT);
	VoC_shr32_ri(ACC1,12,DEFAULT);
	

	VoC_bnez32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L4,ACC1)
		VoC_lw16i_short(ACC1_LO,1,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L4:
	VoC_push32(ACC1,DEFAULT);
	// Compute Noise Power Reverse

	VoC_movreg32(REG23,ACC1,DEFAULT);
	VoC_lw16i_short(REG4,21,IN_PARALLEL);

	//REG23:NoisePowerEst_r.d[in]
	//REG4:NoisePowerEst_r.t[in]
	//ACC0:NoisePowerEst_r.d[out]
	//REG3:[out]	
	VoC_jal(Coolsand_ShrinkBit);

	//VoC_movreg32(REG45,ACC0,DEFAULT);
	VoC_pop32(ACC1,DEFAULT);//NoisePowerEst_r2
	VoC_lw16i_short(REG4,1,IN_PARALLEL);

	VoC_pop32(RL7,DEFAULT);//InstantErrPower
	VoC_lw16i_short(REG5,0,IN_PARALLEL);

	// Update Post SNR
	VoC_movreg32(RL6,RL7,DEFAULT);
	VoC_shr32_ri(RL7,-9,IN_PARALLEL);

	VoC_shr32_rr(RL7,REG3,DEFAULT);
	
	VoC_add32_rr(RL7,RL7,REG45,DEFAULT);
	VoC_movreg32(REG45,ACC0,IN_PARALLEL);

	VoC_shr32_ri(RL7,1,DEFAULT);
	VoC_shr32_ri(ACC0,31,IN_PARALLEL);

	VoC_xor32_rr(RL7,ACC0,DEFAULT);	

	VoC_sub32_rr(RL7,RL7,ACC0,DEFAULT);
	VoC_sub32_rr(ACC0,RL6,ACC1,IN_PARALLEL);


	VoC_bnltz32_r(Coolsand_ComputeNoiseWeight_DeciDirect_L5,ACC0)
		VoC_lw32z(ACC0,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L5:

	
	VoC_shr32_ri(RL7,-16,DEFAULT);
	VoC_shr32_ri(ACC0,-12,IN_PARALLEL);
	
	VoC_lw16i(REG0,g_PF_PostSToNRatio);
	VoC_lw16_d(REG7,g_ch);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_shr16_ri(REG7,-1,IN_PARALLEL);

	VoC_lw16i(REG2,r_PF_PrevSigPowerNBuf);	
	VoC_lw16_d(REG1,m_AECProcPara_SNRUpdateFactor);

	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_shr32_ri(ACC0,12,IN_PARALLEL);
	
	VoC_sw16_p(RL7_HI,REG0,DEFAULT);
	VoC_movreg32(RL7,ACC0,IN_PARALLEL);

	// Update Post Sig Power

	//ACC1:pPrevSigPowerN[in]
	//RL7:PostSigPower[in]
	//REG1:SNRUpdateFactor[in]

	//ACC0:PriorSigPower[out]

	//VoC_shr32_ri(RL7,-4,DEFAULT);

//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	VoC_sub32_rp(REG45,RL7,REG2,DEFAULT);
	VoC_movreg32(RL6,REG45,IN_PARALLEL);
	
	VoC_shr32_ri(REG45,-8,DEFAULT);
	VoC_lw32_p(ACC0,REG2,IN_PARALLEL);
	
	VoC_shr32_ri(REG45,7,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
	
	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);
	
	VoC_mac32_rr(REG4,REG1,DEFAULT);
	
	VoC_macX_rr(REG5,REG1,DEFAULT);	
	VoC_lw16i_short(REG2,20,IN_PARALLEL);//xuml

	//RL6:NoisePowerEst_r.d.[in]
	//REG2:NoisePowerEst_r.t-1.[in]

	//REG3: NoisePowerFirstBitPos.[out]
	VoC_jal(Coolsand_MSBPos);

	VoC_lw16i(REG1,g_PF_NoiseWeight);	
	VoC_lw16_d(REG7,g_ch);

	VoC_lw16i_short(REG4,8,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);	
	
	VoC_sub16_rr(REG0,REG3,REG4,DEFAULT);
	
	VoC_shr32_rr(ACC0,REG0,DEFAULT);	
	VoC_lw16_p(REG3,REG1,IN_PARALLEL);

	VoC_shr32_ri(ACC0,-16,DEFAULT);	
	VoC_shr32_ri(REG23,16,IN_PARALLEL);

	VoC_push16(REG3,DEFAULT);	

	VoC_movreg16(REG0,ACC0_HI,DEFAULT);
	VoC_lw16i_short(REG4,9,IN_PARALLEL);
	
	//REG23:NoiseWeight_r.d[in]
	//REG4:NoiseWeight_r.t[in]
	//ACC0:NoiseWeight_r.d[out]
	//REG3:[out]

	VoC_jal(Coolsand_ShrinkBit);

	VoC_movreg16(REG2,ACC0_LO,DEFAULT);
	VoC_lw16i_short(REG5,1,IN_PARALLEL);

	//NoiseWeight_r.d  = NoiseWeight_r.d*2;
	VoC_shr16_ri(REG2,-1,DEFAULT);
	VoC_shr16_ri(REG5,-7,IN_PARALLEL);

	//Mul_SNRWeight = *pPriorSToNRatio;
	VoC_movreg16(REG4,REG0,DEFAULT);
	VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L6,REG5,REG2)
		VoC_multf16_rr(REG4,REG2,REG0,DEFAULT);		
Coolsand_ComputeNoiseWeight_DeciDirect_L6:

	VoC_pop16(REG2,DEFAULT);

	VoC_add16_rr(REG4,REG4,REG2,DEFAULT);

//	VoC_lw16i_short(RL6_HI,1,DEFAULT);
//	VoC_bez16_r(Coolsand_ComputeNoiseWeight_DeciDirect_L7,REG4)
//		VoC_movreg16(RL6_HI,REG4,DEFAULT);
//Coolsand_ComputeNoiseWeight_DeciDirect_L7:

	VoC_movreg16(RL6_HI,REG4,DEFAULT);

	VoC_shr32_ri(RL6,16,DEFAULT);
	VoC_lw16i_short(REG2,16,IN_PARALLEL); //xuml

	//RL6:Mul_SNRWeight.d.[in]
	//REG2:Mul_SNRWeight.t-1.[in]

	//REG3: MulSNRWeightFirstBitPos.[out]
	VoC_jal(Coolsand_MSBPos);

	VoC_lw16i_short(FORMAT16,-16,DEFAULT);

	VoC_lw16_d(REG7,g_ch);

	VoC_lw16_d(REG1,m_AECProcPara_NoiseOverEstFactor);

	VoC_lw16i_short(REG0,15,DEFAULT);
	VoC_lw16i_short(REG2,1,IN_PARALLEL);


	VoC_sub16_rr(REG0,REG3,REG0,DEFAULT);
	VoC_shr16_ri(REG2,-7,IN_PARALLEL);

	VoC_lw16i(REG3,g_PF_NoiseWeight);
	VoC_lw16i(REG5,g_PF_NoiseAmpWeight);

	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

	//	DynamicNoiseGainLimit = NoiseGainLimitStep*ch;
	VoC_mult16_rd(REG7,REG7,NoiseGainLimitStep_ADDR);
	
	VoC_shr16_rr(REG1,REG0,DEFAULT);
	VoC_lw16i_short(REG4,0,IN_PARALLEL);

	VoC_add16_rd(REG7,REG7,m_AECProcPara_NoiseGainLimit);

	VoC_shr16_ri(REG7,-8,DEFAULT);
	
	VoC_shr16_ri(REG7,8,DEFAULT);

	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L8,REG2,REG1)
		VoC_sub16_rr(REG4,REG2,REG1,DEFAULT);		
Coolsand_ComputeNoiseWeight_DeciDirect_L8:

	VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L11,REG4,REG7)
		VoC_movreg16(REG4,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L11:

	VoC_lw16_d(REG2,AmpThr_ADDR);
	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG2,REG7)
		VoC_movreg16(REG2,REG7,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:


	VoC_movreg16(REG6,REG4,DEFAULT);
	VoC_shr16_ri(REG6,-4,DEFAULT);
	
	VoC_bngt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG4,REG2)
		VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);
Coolsand_ComputeNoiseWeight_DeciDirect_L10:

*/


/*

	VoC_lw16_d(REG7,NoiseWeight_threshold_ADDR);
	VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
	VoC_shr16_ri(REG6,-4,DEFAULT);

	

	VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG7,REG4)
		VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);
		VoC_jump(Coolsand_ComputeNoiseWeight_DeciDirect_L10);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:

	VoC_bngt16_rd(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG4,m_AECProcPara_NoiseGainLimit)
		VoC_movreg16(REG6,REG4,DEFAULT);
		VoC_shr16_ri(REG6,-4,DEFAULT);
Coolsand_ComputeNoiseWeight_DeciDirect_L10:


	*/




/*	

	VoC_multf16_rd(REG6,REG4,m_AECProcPara_AmpGain);

	VoC_bgt16_rd(Coolsand_ComputeNoiseWeight_DeciDirect_L9,REG4,m_AECProcPara_NoiseGainLimit)
		VoC_lw16_d(REG4,m_AECProcPara_NoiseGainLimit);
Coolsand_ComputeNoiseWeight_DeciDirect_L9:

		VoC_lw16_d(REG7,m_AECProcPara_NoiseGainLimit);
		VoC_shr16_ri(REG7,-4,DEFAULT);
	VoC_bgt16_rr(Coolsand_ComputeNoiseWeight_DeciDirect_L10,REG6,REG7)
		VoC_movreg16(REG6,REG7,DEFAULT);		
Coolsand_ComputeNoiseWeight_DeciDirect_L10:

  */

  
/*
	// Update Post SNR	
	VoC_pop16(RA,DEFAULT);
	VoC_sw16_p(REG4,REG3,DEFAULT);
	VoC_sw16_p(REG6,REG5,DEFAULT);
	VoC_return;
}
*/


/**************************************************************************************
 * Function:    Coolsand_CmplxAppxNorm
 *
 * Description:  no
 *
 * Inputs:       
 *   REG0: *pErrDataI
 *	 REG1: *pErrDataQ
 *
 * Outputs: 
 *	 ACC0: *pNorm
 *    
 * Used : REG0,REG1,REG2,REG3,ACC0.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/
void Coolsand_CmplxAppxNorm(void)
{

	//AbsI = ABS(*pSrcI);
	//AbsQ = ABS(*pSrcQ);
	VoC_movreg32(REG23,REG01,DEFAULT);
	VoC_shr16_ri(REG0,15,IN_PARALLEL);

	VoC_xor16_rr(REG2,REG0,DEFAULT)
	VoC_shr16_ri(REG1,15,IN_PARALLEL);

	VoC_xor16_rr(REG3,REG1,DEFAULT);
	VoC_sub16_rr(REG2,REG2,REG0,IN_PARALLEL);

	VoC_sub16_rr(REG3,REG3,REG1,DEFAULT);
	VoC_lw16i_short(REG0,31,IN_PARALLEL);
	
	VoC_bgt16_rr(Coolsand_CmplxAppxNorm_L0,REG2,REG3)

		VoC_movreg16(REG2,REG3,DEFAULT);
		VoC_movreg16(REG3,REG2,IN_PARALLEL);

Coolsand_CmplxAppxNorm_L0:

	VoC_lw16i_short(FORMAT32,5,DEFAULT);
	VoC_lw16i_short(FORMATX,3,IN_PARALLEL);		

	VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT);
	VoC_lw16i_short(REG1,3,IN_PARALLEL);

	VoC_macX_rr(REG3,REG1,DEFAULT);
	
	VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_FirstOrderIIRSmooth2
 *
 * Description:  no
 *
 * Inputs:       
 *	 ACC0: *pSmoothErrPower
 *   RL7: InstantErrPower
 *	 REG1: ErrSmoothFactor
 *
 * Outputs: 
 *	 ACC0: *pSmoothErrPower
 *    
 * Used : REG1,REG4,REG5,RL7,ACC0,ACC1.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/
void Coolsand_FirstOrderIIRSmooth2(void)
{
	
	VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);

	VoC_shr32_ri(REG45,-8,DEFAULT);
//	VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG45,7,DEFAULT);
	//VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

	VoC_shru16_ri(REG4,1,DEFAULT);
	//VoC_lw16i_short(FORMATX,-4,IN_PARALLEL);

	VoC_mac32_rr(REG4,REG1,DEFAULT);

	VoC_macX_rr(REG5,REG1,DEFAULT);	
		
	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_NoisePSDEst
 *
 * Description:  no
 *
 * Inputs:       
 * 
 *	REG0:pSigPower(address)
 *	REG1:pNoisePower(address)
 *	REG2:NoiseUpdateFactor
 *  REG3:pInstantSNR
 *	REG4:UpdateSpeed0
 *	REG5:UpdateSpeed1
 *
 *  REG6:FrameCnt
 *
 * Outputs: no
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 * Version 2.0  Created by  Xuml       04/21/2011
 **************************************************************************************/
void Coolsand_NoisePSDEst(void)
{


	VoC_push16(RA,DEFAULT);

	VoC_lw16i(REG7,50);

	VoC_lw32_p(ACC0,REG0,DEFAULT);//pSigPower
	VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower

	VoC_bngt16_rr(Coolsand_NoisePSDEst_L0,REG7,REG6)
		
		VoC_movreg32(ACC1,ACC0,DEFAULT);

		VoC_jump(Coolsand_NoisePSDEst_Lx);
Coolsand_NoisePSDEst_L0:

		VoC_bngt32_rr(Coolsand_NoisePSDEst_L1,ACC1,ACC0)

			VoC_bez32_r(Coolsand_NoisePSDEst_L1_sub0,ACC0)

				VoC_movreg32(ACC1,ACC0,DEFAULT);

				VoC_jump(Coolsand_NoisePSDEst_Lx);
Coolsand_NoisePSDEst_L1_sub0:

				VoC_lw16i_short(ACC1_LO,1,DEFAULT);
				VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

			VoC_jump(Coolsand_NoisePSDEst_Lx);


Coolsand_NoisePSDEst_L1:


			

		VoC_lw16i_short(RL6_LO,16,DEFAULT);
		VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);

		VoC_bgt32_rr(Coolsand_NoisePSDEst_L2,ACC1,RL6)
			
			VoC_movreg16(REG7,REG4,DEFAULT);
			VoC_jump(Coolsand_NoisePSDEst_L4);
Coolsand_NoisePSDEst_L2:

		VoC_lw16i_short(RL6_HI,0,DEFAULT);
		VoC_lw16i_short(RL6_LO,32,IN_PARALLEL);

		VoC_bngt32_rr(Coolsand_NoisePSDEst_L3,RL6,ACC1)
			VoC_movreg16(REG7,REG5,DEFAULT);
			VoC_jump(Coolsand_NoisePSDEst_L4);
Coolsand_NoisePSDEst_L3:

		VoC_shr16_ri(REG3,-10,DEFAULT);
		VoC_shr16_ri(REG3,10,DEFAULT);
		VoC_lw16i(REG4,AdaptTable);
		VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
		VoC_NOP();
		VoC_lw16_p(REG7,REG4,DEFAULT);

Coolsand_NoisePSDEst_L4:


		VoC_multf16_rr(REG3,REG2,REG7,DEFAULT);
		VoC_lw16i_short(FORMAT16,4-16,IN_PARALLEL);	
		
		VoC_lw16_d(REG4,pInstantSNR_t);
		
		VoC_push32(ACC0,DEFAULT);
		VoC_shr16_ri(REG3,-4,IN_PARALLEL);

		VoC_shr32_ri(REG23,4+16,DEFAULT);
		VoC_push16(REG1,IN_PARALLEL);


		//REG23:AdptNoiseUpdateFactor.d[in]
		//REG4:AdptNoiseUpdateFactor.t[in]
		//ACC0:AdptNoiseUpdateFactor.d[out]

	
		VoC_jal(Coolsand_ShrinkBit);

		VoC_pop16(REG1,DEFAULT);

		VoC_pop32(RL7,DEFAULT);
		VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);

		VoC_bez32_r(Coolsand_NoisePSDEst_Lx,RL7)
			
			//ACC1:pNoisePowerEst[in]
			//RL7:pInstantErrPower[in]
			//REG7:AdptNoiseUpdateFactor[in]

			//ACC0:*pSmoothErrPower[out]

		//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

			VoC_lw16_d(REG2,pInstantSNR_f);

			VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

			VoC_shr32_ri(REG45,-8,DEFAULT);
			VoC_lw16i_short(REG3,15,IN_PARALLEL);
			

			VoC_shr32_ri(REG45,7,DEFAULT);
			VoC_sub16_rr(REG3,REG2,REG3,IN_PARALLEL);

			VoC_shru16_ri(REG4,1,DEFAULT);
			
			//VoC_lw16i_short(FORMAT32,11,DEFAULT);
			VoC_movreg16(FORMAT32,REG2,DEFAULT);
			VoC_mac32_rr(REG4,REG7,IN_PARALLEL);

			//VoC_lw16i_short(FORMATX,11-15,DEFAULT);
			VoC_movreg16(FORMATX,REG3,DEFAULT);
			VoC_macX_rr(REG5,REG7,IN_PARALLEL);	

Coolsand_NoisePSDEst_Lx:

	VoC_lw16i_short(RL6_LO,4,DEFAULT);
	VoC_lw16i_short(RL6_HI,0,IN_PARALLEL);

	VoC_bgt32_rr(Coolsand_NoisePSDEst_END,ACC1,RL6)
		VoC_movreg32(ACC1,RL6,DEFAULT);		
Coolsand_NoisePSDEst_END:

	VoC_pop16(RA,DEFAULT);
	VoC_sw32_p(ACC1,REG1,DEFAULT);
	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_ShrinkBit
 *
 * Description:  no
 *
 * Inputs:       
 *	REG23:
 *  REG4:
 *
 * Outputs: ACC0,REG3
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/23/2010
 **************************************************************************************/

void Coolsand_ShrinkBit(void)
{
	
	VoC_movreg32(RL6,REG23,DEFAULT);
	VoC_shr32_ri(REG23,31,IN_PARALLEL);
	
	VoC_movreg32(RL7,REG23,DEFAULT);	
	VoC_movreg16(REG2,REG4,IN_PARALLEL);

	VoC_push16(RA,DEFAULT);
	VoC_push32(RL6,IN_PARALLEL);


	//RL6:tempData
	//REG2:BitNum-1

	//REG3: FirstBitPos
	VoC_jal(Coolsand_MSBPos);

	VoC_pop16(RA,DEFAULT);
	VoC_pop32(RL6,IN_PARALLEL);

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw16i_short(REG4,1,IN_PARALLEL);

	VoC_bez16_r(Coolsand_ShrinkBit_L0,REG3)

		VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
		VoC_lw16i_short(REG5,0,IN_PARALLEL);

		VoC_xor32_rr(RL6,RL7,DEFAULT);
		VoC_shr32_rr(REG45,REG2,IN_PARALLEL);

		VoC_sub32_rr(RL6,RL6,RL7,DEFAULT);

		VoC_and32_rr(REG45,RL6,DEFAULT);

		VoC_xor32_rr(REG45,RL7,DEFAULT);

		VoC_sub32_rr(ACC0,REG45,RL7,DEFAULT);	

Coolsand_ShrinkBit_L0:

	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_Lin2Log
 *
 * Description:  no
 *
 * Inputs:       
 *  REG01:
 *
 * Outputs: REG01
 *	 
 *    
 * Used : REG0/1/2/3/4/5/6/7, RL6, ACC0
 *
 * NOTE: no.
 *
 * Version 1.0        07/19/2016
 **************************************************************************************/

void Coolsand_Lin2Log(void)
{
	
	//input Temp1 :REG01, Q31
	
	//S32 Temp2
	VoC_lw32z(REG67,DEFAULT);
	VoC_lw16i(REG6,32);
	
	VoC_bnez32_r(Coolsand_Lin2Log1,REG01);
	VoC_jump(Coolsand_Lin2Log2);

Coolsand_Lin2Log1:
	//S32 sv
	VoC_lw32z(RL6,DEFAULT);
	////0x40000000
	VoC_lw32z(REG45,DEFAULT);
	VoC_lw16i(REG5,0x4000);
	//0x1
	VoC_lw32z(REG23,DEFAULT); 
	VoC_lw16i(REG2,1);
	while_start:
	VoC_sub32_rr(ACC0,REG01,REG45,DEFAULT);
	VoC_bngtz32_r(while_end,REG01);
	VoC_bnltz32_r(while_end,ACC0);
			VoC_shr32_ri(REG01,-1,DEFAULT);
			VoC_add32_rr(RL6,RL6,REG23,DEFAULT);
			VoC_jump(while_start);
    while_end:


	VoC_movreg32(REG67,RL6,DEFAULT);
	VoC_shr32_ri(REG01,26,DEFAULT);

	VoC_lw16i(REG2,0xF);
	VoC_lw16i(REG3,r_HF_ROM);
	
	VoC_and16_rr(REG0,REG2,DEFAULT);
	VoC_NOP();
	VoC_add16_rr(REG3,REG3,REG0,DEFAULT);
	VoC_NOP();
	VoC_lw16_p(REG0,REG3,DEFAULT);
	
Coolsand_Lin2Log2:
	
	VoC_shr32_ri(REG67,-5,DEFAULT);
	VoC_NOP();
	VoC_add32_rr(REG01,REG01,REG67,DEFAULT);
	VoC_lw32z(REG23,DEFAULT);
	VoC_NOP();
	VoC_sub32_rr(REG01,REG23,REG01,DEFAULT);

	VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_PostFiltering
 *
 * Description:  no
 *
 * Inputs:       
 *	
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/25/2010
 * Version 2.0  Created by  Xuml       05/04/2011
 **************************************************************************************/

void Coolsand_PostFiltering(void)
{
	VoC_push16(RA,DEFAULT);

	VoC_lw16_d(REG7,g_ch);

	VoC_lw16i(REG0,g_PF_EchoWeight);
	
	VoC_lw16i(REG1,g_PF_NoiseWeight);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

	//VoC_lw16_d(REG3,m_AECProcPara_NoiseGainLimitStep);

	VoC_lw16_p(REG0,REG0,DEFAULT);

	VoC_lw16_p(REG1,REG1,DEFAULT);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		//printf("MulWeight:%d\n",REGS[0].reg);
		//printf("DynamicNoiseGainLimit:%d\n",REGS[3].reg);
		
		fprintf(LogFileHandle,"*pNoiseWeight:%d; *pEchoWeight:%d\n",REGS[1].reg, REGS[0].reg);

	}
#endif
	// VoC_directive: PARSER_ON

	//VoC_mult16_rr(REG3,REG3,REG7,DEFAULT);
	//VoC_mult16_rr(REG0,REG0,REG1,IN_PARALLEL);
	VoC_mult16_rr(REG0,REG0,REG1,DEFAULT);

	VoC_lw16i(REG4,127);

	//VoC_add16_rd(REG3,REG3,m_AECProcPara_NoiseGainLimit);

	//VoC_bngt16_rr(Coolsand_PostFiltering_L0,REG4,REG3)

	//	VoC_movreg16(REG4,REG3,DEFAULT);
	
//Coolsand_PostFiltering_L0:

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		//printf("MulWeight:%d\n",REGS[0].reg);
		//printf("DynamicNoiseGainLimit:%d\n",REGS[3].reg);
		
		fprintf(LogFileHandle,"MulWeight:%d\n",REGS[0].reg);
		//fprintf(LogFileHandle,"DynamicNoiseGainLimit:%d\n",REGS[3].reg);

	}
#endif
	// VoC_directive: PARSER_ON
	VoC_shr16_ri(REG4,-7,DEFAULT);

	VoC_bngt16_rr(Coolsand_PostFiltering_L1,REG0,REG4)

		VoC_mult16_rd(REG1,REG1,m_AECProcPara_AmpGain);//*pNoiseAmpWeight = (*pNoiseWeight)*AmpGain;
		VoC_jump(Coolsand_PostFiltering_L2)	;
Coolsand_PostFiltering_L1:	
		VoC_shr16_ri(REG1,-4,DEFAULT);
		VoC_NOP();
Coolsand_PostFiltering_L2:	

	VoC_lw16i(REG2,g_PF_NoiseAmpWeight);

	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

	VoC_lw16i(REG3,g_PF_EchoWeight);
	
	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		fprintf(LogFileHandle,"pNoiseAmpWeight:%d\n",REGS[1].reg);
	}
#endif
	// VoC_directive: PARSER_ON

	VoC_sw16_p(REG1,REG2,DEFAULT);
	VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);
	
	VoC_lw16i(REG0,g_AF_ErrDataBufI);
	VoC_lw16i(REG1,g_AF_ErrDataBufQ);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

	VoC_lw16i_short(FORMAT16,7-16,DEFAULT);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG1,DEFAULT);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"ErrData\n");
		fprintf(LogFileHandle,"pErrDataI:%d\n",REGS[0].reg);
		fprintf(LogFileHandle,"pErrDataQ:%d\n",REGS[1].reg);
	}
#endif
	// VoC_directive: PARSER_ON

	VoC_multf16_rp(REG0,REG0,REG3,DEFAULT);
	VoC_multf16_rp(REG1,REG1,REG3,IN_PARALLEL);

	VoC_push32(REG01,DEFAULT);//save (*pErrDataI) and (*pErrDataQ)

	VoC_movreg32(RL7,REG01,DEFAULT);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"tempOut = ErrData * EchoWeight\n");
		fprintf(LogFileHandle,"tempOutI:%d\n",REGS[0].reg);
		fprintf(LogFileHandle,"tempOutQ:%d\n",REGS[1].reg);
	}
#endif
	// VoC_directive: PARSER_ON

	//REG0:pOutDataI;[in]
	//REG1:pOutDataQ;[in]
	//ACC0:OutNorm[out]
	
	VoC_jal(Coolsand_CmplxAppxNorm);

	VoC_shr32_ri(ACC0,-16,DEFAULT);

	VoC_movreg16(REG0,ACC0_HI,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
	VoC_movreg16(REG3,REG7,IN_PARALLEL);

	VoC_lw16i(REG1,r_PF_PrevSigPowerEBuf);

	VoC_shr16_ri(REG3,-1,DEFAULT);
	VoC_lw16i_short(FORMAT32,11,IN_PARALLEL);

	VoC_add16_rr(REG1,REG1,REG3,DEFAULT);

	// Noise Amp Weight
	VoC_lw16i(REG2,g_PF_NoiseAmpWeight);

	VoC_sw32_p(ACC0,REG1,DEFAULT);//	pPrevSigPowerE->d = PrevSigPower.d;

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"pPrevSigPowerE:%d\n",REGL[0]);
	}
#endif
	// VoC_directive: PARSER_ON

	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_movreg32(REG01,RL7,IN_PARALLEL);

	VoC_lw16i_short(FORMAT16,11-16,DEFAULT);

	VoC_multf16_rp(REG0,REG0,REG2,DEFAULT);
	VoC_multf16_rp(REG1,REG1,REG2,IN_PARALLEL);

	VoC_NOP();
	
	VoC_movreg32(RL7,REG01,DEFAULT);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"pOutDataI:%d\n",REGS[0].reg);
		fprintf(LogFileHandle,"pOutDataQ:%d\n",REGS[1].reg);
	}
#endif
	// VoC_directive: PARSER_ON
	//REG0:pOutDataI;[in]
	//REG1:pOutDataQ;[in]
	//ACC0:OutNorm[out]
	
	VoC_jal(Coolsand_CmplxAppxNorm);

	VoC_shr32_ri(ACC0,-16,DEFAULT);

	VoC_lw16i(REG2,g_PF_NoiseWeight);

	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_pop32(REG01,IN_PARALLEL);

	VoC_push16(ACC0_HI,DEFAULT);
	VoC_lw16i_short(FORMAT16,7-16,IN_PARALLEL);

	VoC_multf16_rp(REG0,REG0,REG2,DEFAULT);
	VoC_multf16_rp(REG1,REG1,REG2,IN_PARALLEL);

	VoC_lw16i(REG4,r_PF_PrevSigPowerNBuf);
//	VoC_lw16i(REG5,r_PF_PrevSigPowerEBuf);

	VoC_shr16_ri(REG7,-1,DEFAULT);
	
	//REG0:pOutDataI;[in]
	//REG1:pOutDataQ;[in]
	//ACC0:OutNorm[out]
	
	VoC_jal(Coolsand_CmplxAppxNorm);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{
		VoC_NOP();
		VoC_NOP();

		if (REGL[0] > 32767)
			REGL[0] = 32767;
		else if (REGL[0] < -32768)
			REGL[0] = -32768;
		
		fprintf(LogFileHandle,"PrevSigNorm:%d\n",REGL[0]);
	}
#endif
	// VoC_directive: PARSER_ON

	VoC_NOP();

	VoC_shr32_ri(ACC0,-16,DEFAULT);
	VoC_NOP();

	VoC_movreg16(REG0,ACC0_HI,DEFAULT);
	VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);	// modifyed in 0616

	VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);
	VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);
	
	VoC_NOP();
//	VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);	

	//VoC_shr32_ri(ACC1,4,DEFAULT);
//	VoC_movreg32(RL6,ACC1,DEFAULT);
	
	VoC_push32(RL7,DEFAULT);
	VoC_movreg32(RL7,ACC0,DEFAULT);
	VoC_lw32_p(ACC0,REG4,IN_PARALLEL)

	VoC_push16(REG4,DEFAULT);

	VoC_lw16i_short(FORMAT32,11,DEFAULT);
	VoC_lw16i_short(FORMATX,-4,IN_PARALLEL);
	
	VoC_lw16_d(REG1,m_AECProcPara_DataSmoothFactor);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"PrevSigPower:%d\n",REGL[3]);
		fprintf(LogFileHandle,"Old pPrevSigPowerN:%d\n",REGL[0]);
	}
#endif
	// VoC_directive: PARSER_ON

	/*
	PrevSigPower:2094727538
	Old pPrevSigPowerN:596824896
	pPrevSigPowerN:784062726 // C
	pPrevSigPowerN:731042623 // VoC
	*/
	// test
	//VoC_lw16i(ACC0_LO, 0x9928);
	//VoC_lw16i(ACC0_HI, 0xe2a);
	//VoC_lw16i(RL7_LO, 0x2d88);
	//VoC_lw16i(RL7_HI, 0x7d20);

//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);
		VoC_lw16i_short(FORMAT32,0,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_shr32_ri(RL7,-4,DEFAULT);
		VoC_shr32_ri(RL6,11,DEFAULT);
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
//		VoC_shr32_ri(REG45,8,DEFAULT);
		VoC_add32_rr(ACC0,REG45,ACC0,DEFAULT);	//	NewValue = NewValue+(*pOldValue);

	VoC_pop16(REG4,DEFAULT);
	VoC_pop32(RL7,IN_PARALLEL);//pOutDataI pOutDatQ
	VoC_NOP();
	VoC_sw32_p(ACC0,REG4,DEFAULT);


//#ifdef RESIDUL_ECHO_AGC

#if 1
	VoC_lw16_d(REG0,m_AECProcPara_ResidualEchoEnable);
	VoC_bez16_r(RESIDUL_ECHO_AGC_OUT,REG0);


	//REG0:pOutDataI;[in]
	//REG1:pOutDataQ;[in]
	//ACC0:OutNorm[out]
	VoC_movreg32(REG01,RL7,DEFAULT);

	VoC_push32(RL7,IN_PARALLEL); //pOutDataI pOutDataQ
	VoC_jal(Coolsand_CmplxAppxNorm);

	//FixLimit()
	VoC_lw32_d(RL6, AEC_CONST_0x00007fff); //MAX_1
	VoC_lw32_d(RL7, AEC_CONST_0xffff8000); //MAX
	VoC_bngt32_rr(FIX_L1,ACC0,RL6);
		VoC_movreg32(ACC0,RL6,DEFAULT)
		VoC_jump(FIX_L2);
FIX_L1:
	VoC_bngt32_rr(FIX_L2,RL7,ACC0);

		VoC_movreg32(ACC0,RL7,DEFAULT);
FIX_L2:

	VoC_lw16i_short(FORMAT32,-1,DEFAULT);
	VoC_movreg32(REG45,ACC0,DEFAULT);
	VoC_lw16_d(REG7,g_ch);
	VoC_lw16i(REG0,r_PF_pSmoothErrPowerResBuf);
	VoC_shr16_ri(REG7,-1,DEFAULT);
	VoC_multf32_rr(RL7,REG4,REG4,DEFAULT);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_lw16i_short(REG1,4,DEFAULT);
	VoC_lw32_p(ACC0,REG0,DEFAULT);


	VoC_bgt32_rr(RESIDUL_ECHO_AGC1,RL7,ACC0)

		VoC_lw16i_short(REG1,1,DEFAULT);
RESIDUL_ECHO_AGC1:

		VoC_lw16i_short(FORMAT32,-11,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_NOP();
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_NOP();
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_lw16i_short(FORMAT32,4,DEFAULT);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_NOP();
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
		VoC_NOP();
		VoC_add32_rr(ACC1,REG45,ACC0,DEFAULT);	//	NewValue = NewValue+(*pOldValue);
		VoC_NOP();
		VoC_sw32_p(ACC1,REG0,DEFAULT);
	

	//Lin2Log(pSmoothErrPowerRes, &InstantErrPowerdB);
	//input: REG01, Q31, output: REG0
	VoC_movreg32(REG01,ACC1,DEFAULT);

	VoC_jal(Coolsand_Lin2Log);
	VoC_push16(REG0,DEFAULT);

	//if ((State == 0 || State == 2) && *AGCRrrSpkRelation < *SpkSubErrdBSub1Res)
	VoC_lw16i(REG1,2);
	VoC_lw16_d(REG0,m_AECProcPara_AGCRrrSpkRelation);
	VoC_lw16_d(REG7,g_ch);

    VoC_bez16_d(RESIDUL_ECHO_AGC_L0,r_PF_CL_State);
	VoC_be16_rd(RESIDUL_ECHO_AGC_L0,REG1,r_PF_CL_State);
	VoC_pop16(REG0,DEFAULT);
	VoC_jump(RESIDUL_ECHO_AGC_END);

   
RESIDUL_ECHO_AGC_L0:	
	VoC_blt16_rd(RESIDUL_ECHO_AGC_L1,REG0,r_PF_SpkSubErrdBSub1Res);
	VoC_pop16(REG0,DEFAULT);
	VoC_jump(RESIDUL_ECHO_AGC_END);

RESIDUL_ECHO_AGC_L1:

	VoC_lw16i(REG0,r_PF_PrevResEchoErrPowerBuf);
	VoC_shr16_ri(REG7,-1,DEFAULT);
	VoC_lw16_d(REG1,m_AECProcPara_NPowRiseFactor);
	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_movreg32(RL7,ACC1,DEFAULT);
	VoC_lw32_p(ACC0,REG0,DEFAULT);

	//*pPrevErrPowerN = FirstOrderIIRSmooth2(pPrevErrPowerN, pSmoothErrPowerRes, &NPowRiseFactor)
		VoC_lw16i_short(FORMAT32,-4,DEFAULT);		//		Truncate(NewValue, pOldValue->t, pOldValue->f);
		VoC_sub32_rr(REG45,RL7,ACC0,DEFAULT);	//	NewValue = (*pInstantValue) - (*pOldValue);
		VoC_NOP();
		VoC_shr32_ri(REG45,-1,DEFAULT);
		VoC_NOP();
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_multf32_rr(RL7,REG5,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_lw16i_short(FORMAT32,11,DEFAULT);
		VoC_multf32_rr(RL6,REG4,REG1,DEFAULT);	//	NewValue = NewValue*(*pUpdateFactor);
		VoC_NOP();
		VoC_add32_rr(REG45,RL6,RL7,DEFAULT);
		VoC_NOP();
		VoC_add32_rr(ACC0,REG45,ACC0,DEFAULT);	//	NewValue = NewValue+(*pOldValue);
		VoC_NOP();
		VoC_sw32_p(ACC0,REG0,DEFAULT);


	//Lin2Log(pPrevErrPowerN, &ErrPowerNdB)
	//input: REG01, Q31, output: REG0
	VoC_movreg32(REG01,ACC0,DEFAULT);

	VoC_jal(Coolsand_Lin2Log);


	VoC_pop16(REG4,DEFAULT);
	VoC_lw16_d(REG1,m_AECProcPara_AttExtFactor);
	VoC_lw16i(REG2,0x0000);
	VoC_sub16_rr(REG0,REG4,REG0,DEFAULT);

	VoC_sub16_rr(REG3,REG2,REG1,DEFAULT);
	VoC_lw16i(REG2,r_PF_PrevResEchoAttErrdBBuf);
	VoC_shr16_rr(REG0,REG3,DEFAULT);

	VoC_lw16_d(REG7,g_ch);
	VoC_lw16i(REG4,0x0000);
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_lw16_d(REG3,m_AECProcPara_AttRiseFactor);
	VoC_sub16_rp(REG0,REG0,REG2,DEFAULT);


	//FixLimit()
	VoC_lw16i(REG5,0x3ff) //MAX_1
	VoC_lw16i(REG6,0xfc00) //MAX
	VoC_bngt16_rr(FIX_LIMIT_L0,REG0,REG5);
		VoC_movreg16(REG0,REG5,DEFAULT)
		VoC_jump(FIX_LIMIT_L1);
FIX_LIMIT_L0:
	VoC_bngt16_rr(FIX_LIMIT_L1,REG6,REG0);
		VoC_movreg16(REG0,REG6,DEFAULT);
FIX_LIMIT_L1:

	//if (AttRiseFactor > 0)
	VoC_bngtz16_r(RESIDUL_ECHO_AGC_L2,REG3);
		VoC_sub16_rr(REG3,REG4,REG3,DEFAULT);
		VoC_NOP();
		VoC_shr16_rr(REG0,REG3,DEFAULT);
RESIDUL_ECHO_AGC_L2:

	//if (AttExtFactor > 0)
	VoC_bngtz16_r(RESIDUL_ECHO_AGC_L3,REG1);
		VoC_shr16_rr(REG0,REG1,DEFAULT);
RESIDUL_ECHO_AGC_L3:


	VoC_add16_rr(REG0,REG0,REG3,DEFAULT);

	
	//TempPowerdB = TempPowerdB + *pPrevAttErrdB;
	VoC_add16_rp(REG0,REG0,REG2,DEFAULT);

	//FixLimit()
	VoC_lw16i(REG5,0x3ff) //MAX_1
	VoC_lw16i(REG6,0xfc00) //MAX
	VoC_bngt16_rr(FIX_LIMIT_L00,REG0,REG5);
		VoC_movreg16(REG0,REG5,DEFAULT)
		VoC_jump(FIX_LIMIT_L11);
FIX_LIMIT_L00:
		VoC_bngt16_rr(FIX_LIMIT_L11,REG6,REG0);
		VoC_movreg16(REG0,REG6,DEFAULT);
FIX_LIMIT_L11:


	
	VoC_lw16_d(REG3,m_AECProcPara_AddAttDB);
	VoC_sw16_p(REG0,REG2,DEFAULT);

	VoC_lw16_d(REG1,m_AECProcPara_AttExtFactor);
	VoC_lw16i(REG6,0x0000);
	VoC_lw16_d(REG4,m_AECProcPara_MinAttDB);
	VoC_sub16_rr(REG1,REG6,REG1,DEFAULT);
	VoC_lw16_d(REG5,m_AECProcPara_MaxAttDB);
	
	VoC_shr16_rr(REG3,REG1,DEFAULT);
	VoC_shr16_rr(REG4,REG1,DEFAULT);
	VoC_shr16_rr(REG5,REG1,DEFAULT);

	//TempPowerdB = TempPowerdB + TempAddAttDB;
	VoC_add16_rr(REG0,REG0,REG3,DEFAULT);

	//if (TempPowerdB < TempMinAttDB)
	VoC_sub16_rr(REG1,REG0,REG4,DEFAULT);
	VoC_NOP();
	VoC_bnltz16_r(RESIDUL_ECHO_AGC_L4,REG1);
		VoC_movreg16(REG0,REG4,DEFAULT);
RESIDUL_ECHO_AGC_L4:

	//if (TempPowerdB > TempMaxAttDB)
	VoC_lw16i(REG2,r_PF_PrevResEchoWorkErrdBBuf);
	VoC_sub16_rr(REG1,REG0,REG5,DEFAULT);
	VoC_lw16_d(REG7,g_ch);
	VoC_bngtz16_r(RESIDUL_ECHO_AGC_L5,REG1);
		VoC_movreg16(REG0,REG5,DEFAULT);
RESIDUL_ECHO_AGC_L5:

	VoC_add16_rr(REG7,REG2,REG7,DEFAULT);
	VoC_NOP();
	VoC_lw16_p(REG2,REG7,DEFAULT);
	VoC_lw16_d(REG1,m_AECProcPara_WorkFactFactor);

	//pPrevWorkErrdB: Q0,11; TempPowerdB: Q0, 15; WorkFactFactor:Q4,5
	//*pPrevWorkErrdB = FirstOrderIIRSmooth2(pPrevWorkErrdB, &TempPowerdB, &WorkFactFactor);
	//pPrevWorkErrdB:REG2, TempPowerdB:REG0, WorkFactFactor: REG1

	VoC_sub16_rr(REG3,REG0,REG2,DEFAULT);
	VoC_lw16i(FORMAT32,4);
	//VoC_NOP();
	VoC_multf32_rr(REG45,REG3,REG1,DEFAULT);
	VoC_NOP();
	//VoC_NOP();
	

	//FixLimit()
	VoC_lw16i(REG1,0x3ff) //MAX_1
	VoC_add16_rr(REG4,REG4,REG2,DEFAULT);
	VoC_lw16i(REG0,0xfc00) //MAX
	VoC_bngt16_rr(RESIDUL_ECHO_AGC_L6,REG4,REG1);
		VoC_movreg16(REG4,REG1,DEFAULT)
		VoC_jump(RESIDUL_ECHO_AGC_L7);
RESIDUL_ECHO_AGC_L6:
	VoC_bngt16_rr(RESIDUL_ECHO_AGC_L7,REG0,REG4);
		VoC_movreg16(REG4,REG0,DEFAULT);
RESIDUL_ECHO_AGC_L7:
	VoC_lw16_d(REG3,m_AECProcPara_AttExtFactor);
	VoC_sw16_p(REG4,REG7,DEFAULT);
	//TempPowerdB.d = pPrevWorkErrdB->d >> AttExtFactor;
	VoC_shr16_rr(REG4,REG3,DEFAULT);

	VoC_lw16i(REG0,r_Log2Lin_HF_ROM);
	VoC_movreg16(REG1,REG4,DEFAULT);
	//INPUT:REG1,REG4 OUTPUT:REG0
	//Log2Lin(&TempPowerdB, &ResEchoGain);
	VoC_and16_ri(REG4,0x1f);
	VoC_pop32(REG67,DEFAULT);
	VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
	VoC_shr16_ri(REG1,6,DEFAULT);//Temp2
	VoC_lw16_p(REG0,REG0,DEFAULT);//Temp1
	VoC_bez16_r(RESIDUL_ECHO_AGC_L8,REG1);
		VoC_lw16i(REG3,23170);
		VoC_lw16i(FORMAT32,15);
		VoC_multf32_rr(REG01,REG0,REG3,DEFAULT);
RESIDUL_ECHO_AGC_L8:

	VoC_lw16_d(REG1,m_AECProcPara_Log2LinFactor);
	VoC_lw16i(FORMAT32,15);
	VoC_multf32_rr(REG23,REG6,REG0,DEFAULT);
	VoC_multf32_rr(REG45,REG7,REG0,DEFAULT);

	VoC_shr32_rr(REG23,REG1,DEFAULT);
	VoC_shr32_rr(REG45,REG1,DEFAULT);


	VoC_movreg16(RL7_LO,REG2,DEFAULT);
	VoC_movreg16(RL7_HI,REG4,DEFAULT);
	VoC_NOP();


	VoC_push32(RL7,DEFAULT);//pOutDataI pOutDataQ

RESIDUL_ECHO_AGC_END:

	VoC_pop32(RL7,DEFAULT);//pOutDataI pOutDataQ

RESIDUL_ECHO_AGC_OUT:

#endif



	VoC_lw16_d(REG7,g_ch);
	VoC_lw16i(REG1,r_PF_NoisePowerSmoothEstBuf);
	VoC_shr16_ri(REG7,-1,DEFAULT);
	//VoC_NOP();
	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_NOP();
	VoC_lw32_p(ACC1,REG1,DEFAULT);
	VoC_NOP();
	VoC_shr32_ri(ACC1,4,DEFAULT);
	
	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		
		fprintf(LogFileHandle,"pPrevSigPowerN:%d\n",REGL[0]);
	}
#endif
	// VoC_directive: PARSER_ON

	VoC_shr32_ri(ACC1,-4,DEFAULT);
	
	VoC_bgt32_rd(Coolsand_PostFiltering_L3,ACC1,m_AECProcPara_NoiseMin)
		VoC_lw32_d(ACC1,m_AECProcPara_NoiseMin);
Coolsand_PostFiltering_L3:

	VoC_lw16_d(REG2,m_AECProcPara_CNGFactor);

	VoC_movreg32(REG01,ACC1,DEFAULT);

	VoC_shr32_ri(REG01,-1,DEFAULT);
	VoC_lw16i_short(FORMAT32,-8,IN_PARALLEL);

	VoC_shru16_ri(REG0,1,DEFAULT);
	VoC_lw16i_short(FORMATX,-8-15,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG0,REG2,DEFAULT);

	VoC_macX_rr(REG1,REG2,DEFAULT);

	VoC_lw16_d(REG6,m_AECProcPara_NoiseGainLimit);
	
	VoC_shr32_ri(ACC0,8,DEFAULT);

	VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_lw16i_short(REG2,24,IN_PARALLEL);//xuml
	

	//RL6:TargetNoisePower.d
	//REG2:TargetNoisePower.t-1

	//REG3: TargetNoisePowerFirstBitPos
	VoC_jal(Coolsand_MSBPos);

	VoC_lw16i_short(REG0,0,DEFAULT);
	VoC_lw16i_short(REG1,24,IN_PARALLEL);

	VoC_bez16_r(Coolsand_PostFiltering_L5,REG3)

		VoC_sub16_rr(REG2,REG3,REG1,DEFAULT);
		VoC_lw16i_short(REG4,23,IN_PARALLEL);
		
		
		VoC_and16_ri(REG2,0x0001);

		VoC_bez16_r(Coolsand_PostFiltering_L4,REG2)

			VoC_sub16_rr(REG2,REG4,REG3,DEFAULT);
			VoC_lw16i_short(REG4,9,IN_PARALLEL);

			VoC_shr16_ri(REG2,1,DEFAULT);

			VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
			VoC_lw16i_short(REG0,3,IN_PARALLEL);

			VoC_shr16_rr(REG0,REG2,DEFAULT);
		
			VoC_jump(Coolsand_PostFiltering_L5);
Coolsand_PostFiltering_L4:

			VoC_sub16_rr(REG2,REG1,REG3,DEFAULT);
			VoC_lw16i_short(REG4,11,IN_PARALLEL);

			VoC_shr16_ri(REG2,1,DEFAULT);

			VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
			VoC_lw16i_short(REG0,1,IN_PARALLEL);

			VoC_shr16_rr(REG0,REG2,DEFAULT);
Coolsand_PostFiltering_L5:

	VoC_multf32_rr(ACC0,REG0,REG6,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_lw16i(REG2,g_OutSynFilterInBufI);
	VoC_lw16i(REG3,g_OutSynFilterInBufQ);
	
	VoC_lw16_d(REG7,g_ch);
	
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);
		
	VoC_pop16(REG1,DEFAULT);
	VoC_movreg32(REG45,RL7,IN_PARALLEL);

	VoC_lw32z(ACC1,DEFAULT);
		
	VoC_bez32_d(Coolsand_PostFiltering_L8,m_AECProcPara_NoiseMin)
		VoC_lw16i(ACC1_LO,0x5b);
Coolsand_PostFiltering_L8:
	
	VoC_bngt32_rr(Coolsand_PostFiltering_L6,ACC1,ACC0)

		VoC_movreg32(ACC0,ACC1,DEFAULT);

Coolsand_PostFiltering_L6:	

	VoC_shr32_ri(ACC0,-13,DEFAULT);

	VoC_movreg16(REG0,ACC0_HI,DEFAULT);
	VoC_lw16i_short(REG6,2,IN_PARALLEL);

	VoC_shr16_ri(REG0,-1,DEFAULT);
	VoC_movreg16(RL6_LO,REG0,IN_PARALLEL);

	VoC_bne16_rd(Coolsand_PostFiltering_L9,REG6,r_PF_CL_State)	
	VoC_bgt16_rd(Coolsand_PostFiltering_L7,REG6,g_ch)
Coolsand_PostFiltering_L9:		
	VoC_bngt16_rr(Coolsand_PostFiltering_L7,REG0,REG1)

		//REG0[out]
		//REG1[out]
		VoC_jal(Coolsand_Generate);

		VoC_lw16i_short(REG6,1,DEFAULT);
		VoC_movreg16(REG4,RL6_LO,IN_PARALLEL);

		VoC_shr16_ri(REG0,-1,DEFAULT);
		VoC_shr16_ri(REG1,-1,IN_PARALLEL);

		VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);
		VoC_sub16_rr(REG1,REG1,REG6,IN_PARALLEL);

		VoC_mult16_rr(REG4,REG4,REG0,DEFAULT);
		VoC_mult16_rr(REG5,REG4,REG1,IN_PARALLEL);

		VoC_NOP();
		
		VoC_jump(Coolsand_PostFiltering_End);

Coolsand_PostFiltering_L7:
		//REG0[out]
		//REG1[out]
		VoC_jal(Coolsand_Generate);

Coolsand_PostFiltering_End:
	VoC_pop16(RA,DEFAULT);

	VoC_sw16_p(REG4,REG2,DEFAULT);
	VoC_sw16_p(REG5,REG3,DEFAULT);
	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();
		//printf("pOutDataI->d:%d pOutDataQ->d:%d\n",REGS[4].reg, REGS[5].reg);
		//fprintf(LogFileHandle,"pOutDataI->d:%d pOutDataQ->d:%d\n",REGS[4].reg, REGS[5].reg);
	}
#endif
	// VoC_directive: PARSER_ON
	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_Generate
 *
 * Description:     Generate the PN (Random) Sequence
 *
 * Inputs:       
 *	
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       08/25/2010
 **************************************************************************************/

void Coolsand_Generate(void)
{
	
	VoC_lw16i_set_inc(REG0,m_Taps,1);
	VoC_lw16i_set_inc(REG1,m_Registers,1);

	VoC_loop_i_short(CNGTapLen,DEFAULT);
	VoC_lw16i_short(REG7,0,IN_PARALLEL);
	VoC_startloop0
		VoC_add16inc_rp(REG6,REG1,REG0,DEFAULT);
		VoC_NOP();
		VoC_add16_rp(REG7,REG7,REG6,DEFAULT);
	VoC_endloop0


	VoC_lw16_d(ACC1_LO,m_Registers+31);
	VoC_lw16_d(ACC1_HI,m_Registers+26);

	
	VoC_lw16i_set_inc(REG0,m_Registers+CNGRegLen-1,-1);
	VoC_lw16i_set_inc(REG1,m_Registers+CNGRegLen-2,-1);


	VoC_movreg16(REG6,REG7,DEFAULT);
	VoC_shr16_ri(REG7,15,IN_PARALLEL);

	VoC_and16_ri(REG6,0x0001);

	VoC_xor16_rr(REG6,REG7,DEFAULT);
	VoC_lw16inc_p(ACC0_LO,REG1,IN_PARALLEL);

	
	VoC_loop_i_short(CNGRegLen-1,DEFAULT)
	VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);
	VoC_startloop0
		VoC_lw16inc_p(ACC0_LO,REG1,DEFAULT);
		VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);		
	VoC_endloop0

	VoC_sw16inc_p(REG6,REG0,DEFAULT);
	VoC_movreg32(REG01,ACC1,IN_PARALLEL);

	
	VoC_return;



}




/**************************************************************************************
 * Function:    Coolsand_SumInstantPower
 *
 * Description: 
 *
 * Inputs:  REG2:     
 *			REG3:
 *  
 *
 * Outputs:REG7
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/

void Coolsand_SumInstantPower(void)
{
	VoC_push16(RA,DEFAULT);

//	VoC_loop_i_short(1,DEFAULT);
//	VoC_lw32z(ACC1,IN_PARALLEL);
//	VoC_startloop0
		VoC_lw16inc_p(REG0,REG2,DEFAULT);
		VoC_lw16inc_p(REG1,REG3,DEFAULT);

		VoC_jal(Coolsand_InstantPower);	

	//	VoC_add32_rr(ACC1,ACC1,ACC0,DEFAULT);		
//	VoC_endloop0
		
	//			SumMicPower.d = SumMicPower.d/PF_CL_CHNLNUM;
	//			SumSpkPower.d = SumSpkPower.d/PF_CL_CHNLNUM;


	VoC_pop16(RA,DEFAULT);
	VoC_NOP();

	VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_InstantPower
 *
 * Description: 
 *
 * Inputs:  REG0:     
 *			REG1:
 *  
 *
 * Outputs:ACC0
 *	 
 *    
 * Used : REG0,REG1,REG2,REG3,REG4,REG5,ACC0.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/
void Coolsand_InstantPower(void)
{	
	VoC_push16(RA,DEFAULT);

	VoC_push32(REG23,DEFAULT);

	//REG0:
	VoC_jal(Coolsand_CmplxAppxNorm);	

	VoC_pop16(RA,DEFAULT);
	VoC_shr32_ri(ACC0,-16,IN_PARALLEL);

	VoC_movreg16(REG4,ACC0_HI,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG4,REG4,DEFAULT);
	VoC_pop32(REG23,IN_PARALLEL);
	
	VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ControlLogicMergeChnlEnhancedDTD
 *
 * Description: 
 *
 * Inputs:  REG0:     
 *			
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 * Version 1.1  Optimized by  Xuml       11/04/2011
 **************************************************************************************/
void Coolsand_ControlLogicMergeChnlEnhancedDTD(void)
{
	VoC_push16(RA,DEFAULT);
	VoC_push32(ACC0,IN_PARALLEL);

	VoC_jal(Coolsand_ControlLogicCalVarSub);

	VoC_pop32(RL7,DEFAULT);


	VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

	VoC_lw16i(REG6,r_PF_CL_ErrSmoothPower);
		
	// Smooth Spk Power
	VoC_jal(Coolsand_SmoothPeakPower);

	VoC_jal(Coolsand_ControlLogicChnlDecisionEnhanced);

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_ControlLogicSingleChnlEnhanced
 *
 * Description: 
 *
 * Inputs:  REG0:     
 *			
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/22/2011
 **************************************************************************************/
void Coolsand_ControlLogicSingleChnlEnhanced(void)
{
	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(REG0,2,IN_PARALLEL);

	VoC_lw16d_set_inc(REG2,r_PF_CL_DTCountDown,-1);

	VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L1,REG0,r_PF_CL_ChnlState)

		VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
		VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

              //del by yongheng
		//VoC_bgt16_rr(Coolsand_ControlLogicSingleChnl_L0,REG5,REG4)

		VoC_lw16i_short(REG1,2,DEFAULT);
		VoC_movreg16(REG4,REG5,IN_PARALLEL);
		VoC_lw16_d(REG2,m_AECProcPara_PFCLDTHoldTime);
		
		VoC_jump(Coolsand_ControlLogicSingleChnl_End);		

Coolsand_ControlLogicSingleChnl_L0:

		VoC_lw16i_short(REG6,1,DEFAULT);
		VoC_lw16i_short(REG1,0,IN_PARALLEL);

		//VoC_lw16_d(REG1,r_PF_CL_State);

		VoC_lw16i_short(REG2,0,DEFAULT);
		VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);

		VoC_jump(Coolsand_ControlLogicSingleChnl_End);		
Coolsand_ControlLogicSingleChnl_L1:

	VoC_lw16i_short(REG0,1,DEFAULT);

	VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L4,REG0,r_PF_CL_ChnlState)


		VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
		VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

		VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L2,REG5,REG4)
			VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L2:
		
		VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L3,r_PF_CL_DTCountDown)
			VoC_lw16i_short(REG1,1,DEFAULT);
			VoC_lw16i_short(REG4,0,IN_PARALLEL);
			VoC_jump(Coolsand_ControlLogicSingleChnl_End);			

Coolsand_ControlLogicSingleChnl_L3:		
		VoC_lw16i_short(REG1,2,DEFAULT);
		VoC_inc_p(REG2,IN_PARALLEL);
		VoC_jump(Coolsand_ControlLogicSingleChnl_End);		
Coolsand_ControlLogicSingleChnl_L4:


	VoC_lw16i_short(REG0,3,DEFAULT);
	VoC_bne16_rd(Coolsand_ControlLogicSingleChnl_L7,REG0,r_PF_CL_ChnlState)

		VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
		VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

		VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L5,REG5,REG4)
			VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L5:

		VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L6,r_PF_CL_DTCountDown)
			VoC_lw16i_short(REG1,3,DEFAULT);
			VoC_lw16i_short(REG4,0,IN_PARALLEL);
			VoC_jump(Coolsand_ControlLogicSingleChnl_End);			

Coolsand_ControlLogicSingleChnl_L6:		
		VoC_lw16i_short(REG1,2,DEFAULT);
		VoC_inc_p(REG2,IN_PARALLEL);
		VoC_jump(Coolsand_ControlLogicSingleChnl_End);		
Coolsand_ControlLogicSingleChnl_L7:


		VoC_lw16_d(REG4,r_PF_CL_DTCountUp);
		VoC_lw16_d(REG5,m_AECProcPara_PFCLDTStartTime);

		VoC_bngt16_rr(Coolsand_ControlLogicSingleChnl_L8,REG5,REG4)
			VoC_lw16i_short(REG4,0,DEFAULT);
Coolsand_ControlLogicSingleChnl_L8:


	VoC_bnez16_d(Coolsand_ControlLogicSingleChnl_L9,r_PF_CL_DTCountDown)
		VoC_lw16i_short(REG1,0,DEFAULT);
		VoC_lw16i_short(REG4,0,IN_PARALLEL);
		VoC_jump(Coolsand_ControlLogicSingleChnl_End);	
Coolsand_ControlLogicSingleChnl_L9:		
	VoC_lw16i_short(REG1,2,DEFAULT);
	VoC_inc_p(REG2,IN_PARALLEL);
		
Coolsand_ControlLogicSingleChnl_End:

	VoC_pop16(RA,DEFAULT);
	VoC_sw16_d(REG1,r_PF_CL_State);
	VoC_sw16_d(REG2,r_PF_CL_DTCountDown);
	VoC_sw16_d(REG4,r_PF_CL_DTCountUp);

	VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_ControlLogicCalVarSub
 *
 * Description: 
 *
 * Inputs:  RL6:pMicDataPower     
 *			RL7:pSpkDataPower
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/19/2011
 **************************************************************************************/
void Coolsand_ControlLogicCalVarSub(void)
{
	VoC_push16(RA,DEFAULT);
	VoC_push32(RL6,IN_PARALLEL);

//**********************spk****************************//

//	VoC_lw32_d(ACC0,r_PF_CL_SpkSmoothPower);

	VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

//	VoC_push32(ACC0,DEFAULT);

	VoC_lw16i(REG6,r_PF_CL_SpkSmoothPower);
		
	// Smooth Spk Power
	VoC_jal(Coolsand_SmoothPeakPower);

	VoC_lw32_d(ACC0,r_PF_CL_SpkNoise);
	VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

	// Instant Spk SNR
	VoC_jal(Coolsand_InstantSNR); 

	//REG3:InstantSpkSNR

//	VoC_pop32(ACC0,DEFAULT);//PrevSpkSmoothPower
	
	//Spk  Noise Power Est
	VoC_lw16i_short(REG7,12,DEFAULT);
	VoC_lw16i_short(REG2,11,IN_PARALLEL);
	VoC_lw16i(REG0,r_PF_CL_SpkSmoothPower);
	VoC_lw16i(REG1,r_PF_CL_SpkNoise);

	VoC_sw16_d(REG7,pInstantSNR_t);
	VoC_sw16_d(REG2,pInstantSNR_f);
		
	VoC_lw16_d(REG6,m_AF_FrameCnt);
	VoC_lw16i(REG7,r_PF_CL_SpkNoiseFloorDevCnt);
	VoC_lw16_d(REG2,m_AECProcPara_PFCLNoiseUpdateFactor);

	


	VoC_jal(Coolsand_OverallNoiseEst);


//**********************Mic****************************//

	VoC_pop32(RL7,DEFAULT);

//	VoC_lw32_d(ACC0,r_PF_CL_MicSmoothPower);

	VoC_lw16_d(REG7,m_AECProcPara_PFCLDataSmoothFactor);

//	VoC_push32(ACC0,DEFAULT);

	VoC_lw16i(REG6,r_PF_CL_MicSmoothPower);
	
	// Smooth Mic Power
	VoC_jal(Coolsand_SmoothPeakPower);

	VoC_lw32_d(ACC0,r_PF_CL_MicNoise);
	VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

	// Instant Mic SNR
	VoC_jal(Coolsand_InstantSNR);

	//REG3:InstantSpkSNR

//	VoC_pop32(ACC0,DEFAULT);//PrevSpkSmoothPower
	
	//Spk  Noise Power Est
	VoC_lw16i_short(REG7,12,DEFAULT);
	VoC_lw16i_short(REG2,11,IN_PARALLEL);

	VoC_lw16i(REG0,r_PF_CL_MicSmoothPower);
	VoC_lw16i(REG1,r_PF_CL_MicNoise);
	
	
	VoC_sw16_d(REG7,pInstantSNR_t);
	VoC_sw16_d(REG2,pInstantSNR_f);
	VoC_lw16_d(REG6,m_AF_FrameCnt);
	VoC_lw16i(REG7,r_PF_CL_MicNoiseFloorDevCnt);
	VoC_lw16_d(REG2,m_AECProcPara_PFCLNoiseUpdateFactor);

	//Mic Noise Power Est
	VoC_jal(Coolsand_OverallNoiseEst);

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_ControlLogicChnlDecisionEnhanced
 *
 * Description: 
 *
 * Inputs:       
 *			
 *  
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/22/2011
 **************************************************************************************/
void Coolsand_ControlLogicChnlDecisionEnhanced(void)
{

	VoC_lw32_d(REG67,r_PF_CL_MicNoise);

	VoC_push16(RA,DEFAULT);
	VoC_shr32_ri(REG67,-1,IN_PARALLEL);

	VoC_lw16_d(REG5,m_AECProcPara_PFCLThrSigDet);

	VoC_shru16_ri(REG6,1,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
	VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);
	
	VoC_macX_rr(REG7,REG5,DEFAULT);

	VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

	VoC_shr32_ri(ACC1,-4,DEFAULT);
	VoC_lw16i_short(REG4,0,IN_PARALLEL);

	VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L0,ACC1,ACC0)
		VoC_lw16i_short(REG4,1,DEFAULT);
Coolsand_ControlLogicChnlDecisionEnhanced_L0:

	VoC_lw32_d(REG67,r_PF_CL_SpkNoise);

	VoC_sw16_d(REG4,r_PF_CL_ifMicSigOn);
	
	VoC_shr32_ri(REG67,-1,DEFAULT);

	VoC_shru16_ri(REG6,1,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
	VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);
	
	VoC_macX_rr(REG7,REG5,DEFAULT);
	VoC_lw16i_short(REG0,-1,IN_PARALLEL);//*pState = -1;

	VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

	VoC_lw32_d(RL6,r_PF_CL_MicNoise);

	VoC_shr32_ri(ACC1,-4,DEFAULT);
	VoC_lw16i_short(REG4,0,IN_PARALLEL);

	VoC_lw32_d(REG67,r_PF_CL_MicNoise);

	VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L1,ACC1,ACC0)
	VoC_bnlt32_rd(Coolsand_ControlLogicChnlDecisionEnhanced_L1,REG67,r_PF_CL_SpkSmoothPower)
		VoC_lw16i_short(REG4,1,DEFAULT);
		VoC_NOP();
Coolsand_ControlLogicChnlDecisionEnhanced_L1:

	VoC_sw16_d(REG4,r_PF_CL_ifSpkSigOn);

	// Compare Mic and Spk Power
	VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L2,r_PF_CL_ifSpkSigOn)
	VoC_bez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L2,r_PF_CL_ifMicSigOn)

		//Fix2Double(*pSpkSmoothPower) <Fix2Double(ThrNT*(*pErrSmoothPower))
		VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);

		VoC_lw16_d(REG5,m_AECProcPara_PFCLThrNT);

		VoC_shr32_ri(REG67,-1,DEFAULT);
		VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

		VoC_shru16_ri(REG6,1,DEFAULT);
		VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);

		VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);

		VoC_macX_rr(REG7,REG5,DEFAULT);

		VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

		VoC_shr32_ri(ACC1,-4,DEFAULT);

	VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L2,ACC0,ACC1)
		// Near End Single Talk
			VoC_lw16i_short(REG0,1,DEFAULT);//*pState = 1;
			VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);		
Coolsand_ControlLogicChnlDecisionEnhanced_L2:	



	VoC_bnez32_d(Coolsand_ControlLogicChnlDecisionEnhanced_L3,r_PF_CL_MicSmoothPower)
	VoC_bnez32_d(Coolsand_ControlLogicChnlDecisionEnhanced_L3,r_PF_CL_SpkSmoothPower)

		// Near End Single Talk
		VoC_lw16i_short(REG0,3,DEFAULT);//*pState = 3;
		VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);			
Coolsand_ControlLogicChnlDecisionEnhanced_L3:
		

	VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L4,r_PF_CL_ifSpkSigOn)
	VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_L4,r_PF_CL_ifMicSigOn)

		VoC_lw32_d(ACC0,r_PF_CL_MicSmoothPower);
		VoC_lw32_d(ACC1,r_PF_CL_SpkSmoothPower);

	VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L4,ACC0,ACC1)

		// Near End Single Talk
		VoC_lw16i_short(REG0,3,DEFAULT);//*pState = 3;
		VoC_jump(Coolsand_ControlLogicChnlDecisionEnhanced_End);			
Coolsand_ControlLogicChnlDecisionEnhanced_L4:
/*
	VoC_lw32_d(REG67,r_PF_CL_SpkSmoothPower);

	VoC_shr32_ri(REG67,-1,DEFAULT);

	VoC_lw16_d(REG5,m_AECProcPara_PFCLThrDT);

	VoC_shru16_ri(REG6,1,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
	VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);
	
	VoC_macX_rr(REG7,REG5,DEFAULT);

	VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

	VoC_shr32_ri(ACC1,-4,DEFAULT);
	VoC_lw16i_short(REG0,0,IN_PARALLEL);


	VoC_bgt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_L5,ACC1,ACC0)


			VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);
			VoC_lw16_d(REG5,m_AECProcPara_PFCLThrDTDiff);

			VoC_shr32_ri(REG67,-1,DEFAULT);

			VoC_shru16_ri(REG6,1,DEFAULT);
			VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

			VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
			VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);
			
			VoC_macX_rr(REG7,REG5,DEFAULT);

			VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);
			VoC_lw32_d(REG67,r_PF_CL_ErrSmoothPower);

			VoC_shr32_ri(ACC1,-4,DEFAULT);
			VoC_shr32_ri(REG67,-4,IN_PARALLEL);

			VoC_add32_rr(RL6,REG67,ACC0,DEFAULT);//ErrSmoothPower_db+DiffValue
			VoC_sub32_rr(RL7,REG67,ACC0,IN_PARALLEL);//ErrSmoothPower_db-DiffValue

		VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,RL6,ACC1);
		VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,ACC1,RL7);
			
			VoC_lw32_d(ACC0,r_PF_CL_SpkSmoothPower);
			VoC_lw32_d(ACC1,r_PF_CL_MicSmoothPower);

		VoC_bngt32_rr(Coolsand_ControlLogicChnlDecisionEnhanced_End,ACC1,ACC0);

Coolsand_ControlLogicChnlDecisionEnhanced_L5:

		VoC_lw16i_short(REG0,2,DEFAULT);
Coolsand_ControlLogicChnlDecisionEnhanced_End:*/


		VoC_lw16i_short(REG0,2,DEFAULT);
	VoC_bnez16_d(Coolsand_ControlLogicChnlDecisionEnhanced_End,PFCLDTDOut)
		VoC_lw16i_short(REG0,0,DEFAULT);
Coolsand_ControlLogicChnlDecisionEnhanced_End:

	VoC_pop16(RA,DEFAULT);
	VoC_sw16_d(REG0,r_PF_CL_ChnlState);
	VoC_return;
}




/**************************************************************************************
 * Function:    Coolsand_SmoothPeakPower
 *
 * Description: 
 *
 * Inputs:  REG6: pSmoothPower    
 *			REG7: *pSmoothFactor
 *			RL7:*pInstantPower
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_SmoothPeakPower(void)
{

	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);

	VoC_lw32_p(ACC1,REG6,DEFAULT);
	VoC_lw16i_short(ACC0_HI,0x40,IN_PARALLEL);

	VoC_bngt32_rr(Coolsand_SmoothPeakPower_L0,RL7,ACC1)
	VoC_bngt32_rr(Coolsand_SmoothPeakPower_L0,ACC0,RL7)

		VoC_movreg32(ACC0,RL7,DEFAULT);

		VoC_jump(Coolsand_SmoothPeakPower_L1);

Coolsand_SmoothPeakPower_L0:
		
	VoC_movreg16(REG1,REG7,DEFAULT);
	VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

	VoC_lw16i_short(FORMAT32,11,DEFAULT);
	VoC_lw16i_short(FORMATX,11-15,IN_PARALLEL);

	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

Coolsand_SmoothPeakPower_L1:

	VoC_bnltz32_r(Coolsand_SmoothPeakPower_L2,ACC0)
		VoC_lw32z(ACC0,DEFAULT);	
Coolsand_SmoothPeakPower_L2:

	VoC_pop16(RA,DEFAULT);
	VoC_sw32_p(ACC0,REG6,DEFAULT);
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_InstantSNR
 *
 * Description: 
 *
 * Inputs:  ACC0:pNoisePower     
 *			ACC1:pSigPower
 *  
 *
 * Outputs:REG3
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_InstantSNR(void)
{

	VoC_push16(RA,DEFAULT);	
	VoC_shr32_ri(ACC0,4,IN_PARALLEL);

	VoC_bnez32_r(Coolsand_InstantSNR_L0,ACC0)
		VoC_lw16i_short(ACC0_LO,1,DEFAULT);
		VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
Coolsand_InstantSNR_L0:
	VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_lw16i_short(REG2,20,IN_PARALLEL);
	//RL6:NoisePower_r.d
	//REG2:oisePower_r.t

	//REG3:NoisePowerFirstBitPos
	VoC_jal(Coolsand_MSBPos);

	VoC_lw16i_short(REG7,-1,DEFAULT);
//	VoC_lw16i_short(REG2,0x1,IN_PARALLEL);

	//	int InstantSNRShiftVal = (NoisePower_r.f+1-NoisePowerFirstBitPos - pSigPower->f+pInstantSNR->f);
	VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);
//	VoC_lw16i_short(REG7,-7,IN_PARALLEL);	

	VoC_shr32_rr(ACC1,REG3,DEFAULT);
//	VoC_shr16_rr(REG2,REG7,IN_PARALLEL);

	VoC_shr32_ri(ACC1,-24,DEFAULT);

	VoC_shr32_ri(ACC1,24,DEFAULT);
	VoC_pop16(RA,IN_PARALLEL);

	VoC_movreg16(REG3,ACC1_LO,DEFAULT);

	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_OverallNoiseEst
 *
 * Description: 
 *
 * Inputs:  REG0:   pSigPower
 *			REG1:	pNoisePower
 *          REG2:	NoiseUpdateFactor
 *			REG3:	* pInstantSNR
 *			REG6:	FrameCnt
 *			REG7:	pNoiseFloorDevCnt
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/20/2011
 **************************************************************************************/
void Coolsand_OverallNoiseEst(void)
{

	VoC_lw32_p(ACC0,REG1,DEFAULT);	
	VoC_lw16i_short(REG5,16,IN_PARALLEL);

	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(REG4,16,IN_PARALLEL);	

	VoC_push16(REG3,DEFAULT);
	VoC_push32(ACC0,IN_PARALLEL);

	VoC_push16(REG7,DEFAULT);
	VoC_push32(REG01,IN_PARALLEL);

	VoC_jal(Coolsand_NoisePSDEst);

	VoC_pop16(REG7,DEFAULT);//* pInstantSNR
	VoC_pop32(REG01,IN_PARALLEL);
	
	VoC_pop16(REG3,DEFAULT);//pNoiseFloorDevCnt
	VoC_pop32(ACC0,IN_PARALLEL);//PrevNoisePower->d

	VoC_lw32_p(ACC1,REG0,DEFAULT);//pSigPower->d	

	//if (pSigPower->d!=0)
	VoC_bez32_r(Coolsand_OverallNoiseEst_L2,ACC1)

		VoC_lw16i_short(REG5,1,DEFAULT);
		VoC_lw32_p(ACC1,REG1,IN_PARALLEL);//pSigPower->d	


		VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
		VoC_lw16i_short(REG4,32,IN_PARALLEL);

		VoC_bne32_rr(Coolsand_OverallNoiseEst_L0,ACC1,ACC0);
		VoC_bngt16_rr(Coolsand_OverallNoiseEst_L0,REG3,REG4);
			VoC_add16_rp(REG6,REG5,REG7,DEFAULT);		//*pNoiseFloorDevCnt = (*pNoiseFloorDevCnt) + 1;
Coolsand_OverallNoiseEst_L0:

		VoC_lw16i(REG4,500*8);
		VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower->d
		VoC_shr32_ri(ACC1,-1,DEFAULT);
		VoC_bngt16_rr(Coolsand_OverallNoiseEst_L1,REG6,REG4);

			VoC_lw16i_short(REG6,0,DEFAULT);
			VoC_sw32_p(ACC1,REG1,DEFAULT);

Coolsand_OverallNoiseEst_L1:
		VoC_sw16_p(REG6,REG7,DEFAULT);//save pNoiseFloorDevCnt
		
Coolsand_OverallNoiseEst_L2:

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_OverallNoiseEstwState
 *
 * Description: 
 *
 * Inputs:  REG0:   pSigPower
 *			REG1:	pNoisePower
 *          REG2:	NoiseUpdateFactor
 *			REG3:	* pInstantSNR
 *			REG6:	FrameCnt
 *			REG7:	pNoiseFloorDevCnt
 *
 * Outputs:no
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_OverallNoiseEstwState(void)
{
	VoC_lw16_d(REG7,g_ch);

	VoC_shr16_ri(REG7,-1,DEFAULT);
	VoC_push16(RA,IN_PARALLEL);
	
	VoC_lw16i(REG0,r_PF_SmoothErrPower);
	VoC_lw16i(REG1,r_PF_NoisePowerEstBuf);		
		
	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

	VoC_lw16i_short(REG4,3,DEFAULT);
	VoC_push16(REG7,IN_PARALLEL);
	
	VoC_lw32_p(ACC0,REG1,DEFAULT);

	VoC_push32(REG01,DEFAULT);
	VoC_push16(REG3,IN_PARALLEL);

	VoC_push32(ACC0,DEFAULT);

	VoC_bez16_d(Coolsand_OverallNoiseEstwState_L0_subL0,m_AECProcPara_StrongEchoFlag)
	VoC_bne16_rd(Coolsand_OverallNoiseEstwState_L0,REG4,r_PF_CL_State)
	
Coolsand_OverallNoiseEstwState_L0_subL0:	


		//Spk  Noise Power Est
			
		

		VoC_lw16i_short(REG6,12,DEFAULT);
		VoC_lw16i_short(REG2,11,IN_PARALLEL);
			
		VoC_lw16i_short(REG4,1,DEFAULT);
		VoC_lw16i_short(REG5,16,IN_PARALLEL);

		VoC_sw16_d(REG6,pInstantSNR_t);
		VoC_sw16_d(REG2,pInstantSNR_f);

		VoC_lw16_d(REG6,m_AF_FrameCnt);

		VoC_lw16_d(REG2,m_AECProcPara_NoiseUpdateFactor);

		VoC_jal(Coolsand_NoisePSDEst);

Coolsand_OverallNoiseEstwState_L0:

	
	VoC_pop16(REG3,DEFAULT);//* pInstantSNR
	VoC_pop32(ACC0,IN_PARALLEL);//PrevNoisePower->d

	VoC_pop32(REG01,DEFAULT);//pSigPower&pNoisePower
	VoC_pop16(REG4,DEFAULT);//g_ch

	VoC_lw16i(REG0,r_PF_NoisePowerSmoothEstBuf);//r_PF_NoisePowerSmoothEstBuf
	
	VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
	VoC_lw32_p(ACC1,REG0,IN_PARALLEL);//pSigPower->d

	VoC_shr16_ri(REG4,1,DEFAULT);

	VoC_lw16i(REG7,r_PF_NoiseFloorDevCnt);//pNoiseFloorDevCnt

	VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
	
	//if (pSigPower->d!=0)
	VoC_bez32_r(Coolsand_OverallNoiseEstwState_L3,ACC1)

		VoC_lw16i_short(REG5,1,DEFAULT);
		VoC_lw32_p(ACC1,REG1,IN_PARALLEL);//pNoisePower


		VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
		VoC_lw16i_short(REG4,16,IN_PARALLEL);

		VoC_bne32_rr(Coolsand_OverallNoiseEstwState_L1,ACC1,ACC0);
		VoC_bngt16_rr(Coolsand_OverallNoiseEstwState_L1,REG3,REG4);
			VoC_add16_rp(REG6,REG5,REG7,DEFAULT);		//*pNoiseFloorDevCnt = (*pNoiseFloorDevCnt) + 1;
Coolsand_OverallNoiseEstwState_L1:

		VoC_lw16i(REG4,500*16);
		VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower->d
		VoC_shr32_ri(ACC1,-1,DEFAULT);//			pNoisePower->d = pNoisePower->d*2;
		VoC_bngt16_rr(Coolsand_OverallNoiseEstwState_L2,REG6,REG4);

			VoC_lw16i_short(REG6,0,DEFAULT);//*pNoiseFloorDevCnt = 0;
			VoC_sw32_p(ACC1,REG1,IN_PARALLEL);

			VoC_sw32_p(ACC1,REG0,DEFAULT);// *pNoiseSmoothPower = *pNoisePower;

Coolsand_OverallNoiseEstwState_L2:
		VoC_sw16_p(REG6,REG7,DEFAULT);//save pNoiseFloorDevCnt
		
Coolsand_OverallNoiseEstwState_L3:

	//	if (FrameCnt<50)
	VoC_lw16i(REG4,50);
	VoC_lw32_p(ACC0,REG1,DEFAULT);
	VoC_bngt16_rd(Coolsand_OverallNoiseEstwState_L4,REG4,m_AF_FrameCnt)
		VoC_sw32_p(ACC0,REG0,DEFAULT);// *pNoiseSmoothPower = *pNoisePower;
Coolsand_OverallNoiseEstwState_L4:
	
	//if (pNoiseSmoothPower->d == 0)
	VoC_lw32_p(ACC0,REG0,DEFAULT);
	VoC_lw16i_short(REG6,3,IN_PARALLEL);

	VoC_lw16i_short(REG4,2,DEFAULT);
	VoC_lw16i_short(REG5,0,IN_PARALLEL);

	VoC_bnez32_r(Coolsand_OverallNoiseEstwState_L5,ACC0)
		VoC_sw32_p(REG45,REG0,DEFAULT);		
Coolsand_OverallNoiseEstwState_L5:

	VoC_lw32_p(RL7,REG0,DEFAULT);//pNoiseSmoothPower
	VoC_lw16i_short(REG4,3,IN_PARALLEL);

	VoC_lw32_p(ACC1,REG1,DEFAULT);//pNoisePower
	VoC_movreg32(REG67,RL7,IN_PARALLEL);//pNoiseSmoothPower

	VoC_bez16_d(Coolsand_OverallNoiseEstwState_L7,m_AECProcPara_StrongEchoFlag)

	VoC_bne16_rd(Coolsand_OverallNoiseEstwState_End,REG4,r_PF_CL_State)

Coolsand_OverallNoiseEstwState_L7:
		VoC_shr32_ri(REG67,-1,DEFAULT);
		VoC_bgt32_rr(Coolsand_OverallNoiseEstwState_L6,ACC1,REG67);
			VoC_movreg32(RL7,ACC1,DEFAULT);
			VoC_jump(Coolsand_OverallNoiseEstwState_End);		
Coolsand_OverallNoiseEstwState_L6:
		
		VoC_shru16_ri(REG6,1,DEFAULT);
		VoC_lw16i_short(REG5,50,IN_PARALLEL);
		
		VoC_multf32_rr(ACC0,REG6,REG5,DEFAULT);
		VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

		VoC_macX_rr(REG7,REG5,DEFAULT);
		VoC_lw16i_short(FORMATX,-15,IN_PARALLEL);		

		VoC_NOP();

		VoC_bgt32_rr(Coolsand_OverallNoiseEstwState_End,ACC0,ACC1);
			VoC_movreg32(RL7,ACC1,DEFAULT);
Coolsand_OverallNoiseEstwState_End:

	VoC_pop16(RA,DEFAULT);
	VoC_sw32_p(RL7,REG0,DEFAULT);
	VoC_return;
}


/**************************************************************************************
 * Function:    Cooland_InstantPower
 *
 * Description: 
 *
 * Inputs:       
 *   REG0: *pDataI
 *	 REG1: *pDataQ
 *
 * Outputs: 
 *	 ACC0: *pPower
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Cooland_InstantPower(void)
{
	VoC_push16(RA,DEFAULT);

	VoC_jal(Coolsand_CmplxAppxNorm);

	VoC_shr32_ri(ACC0,-16,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
	
	VoC_movreg16(REG0,ACC0_HI,DEFAULT);
	VoC_pop16(RA,IN_PARALLEL);
	
	VoC_multf32_rr(ACC0,REG0,REG0,DEFAULT);//DataPower = DataNorm*DataNorm;

	VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL
 *
 * Description: 
 *
 * Inputs:       
 *			
 *  
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL(void)
{
	VoC_push16(RA,DEFAULT);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_PF_DEBUG
	{   
		static int i = 0;
		VoC_NOP();
		VoC_NOP();
		//printf("/********************AEC_DEBUG:%d***********************/\n",i);
		fprintf(LogFileHandle,"/********************AEC_DEBUG:%d***********************/\n",i++);
		if(i == 21861)
		{
			//printf("bug!\n");
		}
	}
#endif
	// VoC_directive: PARSER_ON
	VoC_lw16_d(REG7,g_ch);
	
	VoC_lw16i(REG2,g_AF_ErrDataBufI);
	VoC_lw16i(REG3,g_AF_ErrDataBufQ);	
			
	VoC_add16_rr(REG2,REG2,REG7,DEFAULT);
	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);
	
	VoC_lw16_p(REG0,REG2,DEFAULT);
	VoC_lw16_p(REG4,REG2,IN_PARALLEL);//save pErrDataI

	VoC_lw16_p(REG1,REG3,DEFAULT);
	VoC_lw16_p(REG5,REG3,IN_PARALLEL);//save pErrDataQ

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"pErrDataI:%d\n",REGS[0].reg);
	fprintf(LogFileHandle,"pErrDataQ:%d\n",REGS[1].reg);
#endif
// VoC_directive: PARSER_ON

	// Compute Instant Err Power
	VoC_jal(Coolsand_CmplxAppxNorm);

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"InstantErrNorm:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON

	//REG4:pErrDataI
	//REG5:pErrDataQ

#ifndef AEC_VOC_ECHO_DEBUG
	VoC_shr32_ri(ACC0,-16,DEFAULT);

	VoC_movreg16(REG6,ACC0_HI,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG6,REG6,DEFAULT);
	
	VoC_NOP();

	VoC_shr32_ri(ACC0,-8,DEFAULT);

	VoC_shr32_ri(ACC0,8,DEFAULT);
#else
// VoC_directive: PARSER_OFF
	// InstantErrNorm is Q(2.15)
	REGL[0] = (REGL[0] * REGL[0]) >> 7;
	if (REGL[0] > 8388607)
		REGL[0] = 8388607;
	else if (REGL[0] < -8388608)
		REGL[0] = -8388608;
// VoC_directive: PARSER_ON
#endif

	VoC_NOP();

	VoC_push32(ACC0,DEFAULT);

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"InstantErrPower:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON

	// Echo Power Estimation 
	VoC_jal(Coolsand_EchoPSDEst_MFCOHFFix);

	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"pEchoPowerEst:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON

	//ACC0:pEchoPowerEst
	// Compute Echo Power Reverse

	VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);
	VoC_lw16_d(REG7,g_ch);
	VoC_lw16i(REG1,g_PF_EchoWeight);

	VoC_shr16_ri(REG7,-1,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);

	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);

	VoC_pop32(RL7,DEFAULT);

	VoC_lw32_p(REG23,REG0,DEFAULT);

	VoC_shr32_ri(REG23,2,DEFAULT);
	
	VoC_lw16i_short(REG4,22,DEFAULT);
	VoC_push32(RL7,IN_PARALLEL);
	
	VoC_jal(Coolsand_ShrinkBit);
	//ACC0:EchoPowerEst_r.d
	//REG3:FirstBitPos

	VoC_lw16i_short(REG4,22,DEFAULT);//FirstBitPos-1-21-4(for high 16 bit).

	VoC_bnez32_r(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L0,ACC0)

		VoC_lw16i_short(REG4,21,DEFAULT);//FirstBitPos-1-21-4(for high 16 bit).

		VoC_lw16i_short(ACC0_LO,1,DEFAULT);
		VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L0:

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"EchoPowerEst_r:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON
	
	VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);
	VoC_pop32(REG67,IN_PARALLEL);

	// Update Post SNR
	VoC_shr32_rr(REG67,REG4,DEFAULT);
	VoC_movreg32(RL6,REG67,IN_PARALLEL);

	VoC_shr32_ri(REG67,16,DEFAULT);  //*pPostSToNRatio
	VoC_push16(REG4,IN_PARALLEL);//EchoPowerEstRev
	
	VoC_shr32_ri(ACC0,-2,DEFAULT);
	VoC_shr32_ri(REG67,-2,IN_PARALLEL);

	VoC_sub32_rr(RL7,RL6,ACC0,DEFAULT);

	VoC_bgtz32_r(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L1,RL7)
		VoC_lw16i_short(RL7_LO,1,DEFAULT);
		VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L1: 

	VoC_lw16_d(REG2,g_ch);

	VoC_shr16_ri(REG2,-1,DEFAULT);
//	VoC_shr32_ri(RL7,4,IN_PARALLEL);
	
	VoC_lw16i(REG3,r_PF_PrevSigPowerEBuf);

	VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
	VoC_shr32_ri(RL7,-8,IN_PARALLEL);		

	VoC_lw16_d(REG1,m_AECProcPara_SERUpdateFactor);

	VoC_lw32_p(ACC1,REG3,DEFAULT);
	VoC_shr32_ri(RL7,8,IN_PARALLEL);

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"pPrevSigPowerE:%d\n",REGL[1]);
	fprintf(LogFileHandle,"PostSigPower:%d\n",REGL[3]);
	//REGL[1] = 0x7d202d;
	//REGL[3] = 0x164cad;
#endif
// VoC_directive: PARSER_ON

//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);
	VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

	VoC_shr32_ri(REG45,-8,DEFAULT);
	VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG45,7,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

	VoC_mac32_rr(REG4,REG1,DEFAULT);

	VoC_macX_rr(REG5,REG1,DEFAULT);	


	//ACC0:
	VoC_pop16(REG4,DEFAULT);//EchoPowerEstRev

	VoC_lw16i(REG0,g_PF_PriorSToERatio);

	VoC_add16_rd(REG0,REG0,g_ch);	

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"PriorSigPower:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON	

	VoC_shr32_rr(ACC0,REG4,DEFAULT);
	VoC_lw16i_short(RL7_HI,0,IN_PARALLEL);

	VoC_lw16i(RL7_LO,0x8000);

	VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);
	VoC_lw32z(RL7,IN_PARALLEL);
	
	VoC_shr32_ri(ACC0,16,DEFAULT);
	VoC_lw16i_short(REG2,16,IN_PARALLEL);

	VoC_lw16i(RL7_LO,128);

	VoC_sw16_p(ACC0_LO,REG0,DEFAULT);
	VoC_movreg32(RL6,ACC0,IN_PARALLEL);
// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	VoC_NOP();
	VoC_NOP();
	fprintf(LogFileHandle,"pPriorSToERatio->d:%d\n",REGL[0]);
#endif
// VoC_directive: PARSER_ON	

	VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//pPriorSToERatio->d+(0x1<<pPriorSToERatio->f)
	
	VoC_jal(Coolsand_MSBPos);
	//REG3:PriorSERFirstBitPos

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	{
		VoC_NOP();
		VoC_NOP();

		fprintf(LogFileHandle,"PriorSERFirstBitPos->d:%d\n",REGS[3].reg);
	}
#endif
// VoC_directive: PARSER_ON

	VoC_lw16i_short(REG0,14,DEFAULT);

	VoC_sub16_rr(REG3,REG3,REG0,DEFAULT);

	VoC_lw16_d(REG1,m_AECProcPara_EchoOverEstFactor);

// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	{   
		VoC_NOP();
		VoC_NOP();

		fprintf(LogFileHandle,"EchoOverEstFactor->d:%d\n",REGS[1].reg);
	}
#endif
// VoC_directive: PARSER_ON

	VoC_shr16_rr(REG1,REG3,DEFAULT);

	VoC_shr16_ri(REG1,-8,DEFAULT);

	VoC_shr16_ri(REG1,8,DEFAULT);

	VoC_lw16i(REG2,128);

	VoC_sub16_rr(REG4,REG2,REG1,DEFAULT);


	// VoC_directive: PARSER_OFF
#ifdef AEC_VOC_ECHO_DEBUG
	{   
		VoC_lw16_d(REG2, r_PF_CL_State);
		VoC_NOP();
		VoC_NOP();

		fprintf(LogFileHandle,"pEchoWeight->d:%d; PriorSERRev.d:%d;  *pState:%d\n",REGS[4].reg, REGS[1].reg, REGS[2].reg);

	}
#endif
	// VoC_directive: PARSER_ON
	VoC_lw16i(REG1,g_PF_EchoWeight);
	
	VoC_add16_rd(REG1,REG1,g_ch);
 /////////// //normal_mode; by yongheng
//	VoC_bez16_d(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End,r_PF_CL_State)
//        VoC_lw16i(REG4,127);
//        VoC_jump(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End); 
////////////
	// TODO: do not know why EchoGainLimit, maybe we should change it to Q1.7
	VoC_bnez16_d(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L3,m_AECProcPara_StrongEchoFlag)

		VoC_shr16_ri(REG4,-2,DEFAULT);
        VoC_lw16i_short(REG2,0x2,IN_PARALLEL);     //add round by yongheng
		VoC_bgt16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L2,REG4,m_AECProcPara_EchoGainLimit)
			VoC_lw16_d(REG4,m_AECProcPara_EchoGainLimit);
		    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);//add round by yongheng
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L2:
		VoC_shr16_ri(REG4,2,DEFAULT);
		//	// Limit the weight
	//	*pEchoWeight = FixMax(*pEchoWeight, EchoGainLimit);

		VoC_bez16_d(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End,r_PF_CL_State)
			VoC_lw16i(REG4,127);
		VoC_jump(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End);
	
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L3:

		//	// Limit the weight
		VoC_lw16i_short(REG2,2,DEFAULT);
		VoC_lw16i_short(REG3,1,IN_PARALLEL);//		AmpLimit = 1;
		
		VoC_lw16i(REG5,m_AECProcPara_pPFEchoGainLimitAmp);

		VoC_add16_rd(REG5,REG5,g_ch);

		VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L4,REG2,r_PF_CL_State)

			VoC_lw16_p(REG3,REG5,DEFAULT);//		AmpLimit = pEchoGainLimitAmp[Channel];
			
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L4:

		VoC_mult16_rd(REG6,REG3,m_AECProcPara_EchoGainLimit);

		VoC_shr16_ri(REG4,-2,DEFAULT);
		VoC_lw16i_short(REG7,0x2,IN_PARALLEL);

		VoC_bgt16_rr(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L5,REG4,REG6)
			VoC_movreg16(REG4,REG6,DEFAULT);
			VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//+0.5
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L5:	

		VoC_shr16_ri(REG4,2,DEFAULT);
		VoC_lw16i_short(REG2,16,IN_PARALLEL);

		VoC_bgt16_rr(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L7,REG4,REG6)
		VoC_mult16_rr(REG4,REG4,REG4,DEFAULT);
		VoC_NOP();
		VoC_shr16_ri(REG4,4,DEFAULT);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L7:
		VoC_lw16i_short(REG2,1,DEFAULT);
		VoC_lw16i_short(REG3,3,IN_PARALLEL);

		VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L6,REG2,r_PF_CL_State)
			VoC_lw16i(REG4,127);
		VoC_jump(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End);			
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_L6:		
		VoC_bne16_rd(Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End,REG3,r_PF_CL_State)
			VoC_lw16i(REG4,127);
Coolsand_ComputeEchoWeight_DeciDirect_MFCOHFCTRL_End:

	VoC_pop16(RA,DEFAULT);
	VoC_sw16_p(REG4,REG1,DEFAULT);
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_EchoPSDEst_MFCOHFFix
 *
 * Description: 
 *
 * Inputs:  REG4: 
 *			REG5:
 *			ACC0:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       04/25/2011
 **************************************************************************************/
void Coolsand_EchoPSDEst_MFCOHFFix(void)
{
	VoC_push16(RA,DEFAULT);
//	VoC_push32(ACC0,IN_PARALLEL);//*pInstantErrPower
	
	VoC_lw16_d(REG2,g_ch);

	VoC_push32(REG45,DEFAULT);//save 
	VoC_shr16_ri(REG2,-3,IN_PARALLEL);//FILTER_LEN
	
	VoC_lw16i(REG6,r_AF_SpkSigBufI);
	VoC_lw16i(REG7,r_AF_SpkSigBufQ);	
			
	VoC_add16_rr(REG6,REG6,REG2,DEFAULT);
	VoC_add16_rr(REG7,REG7,REG2,DEFAULT);

	VoC_lw16_p(REG0,REG6,DEFAULT);
	VoC_lw16i_short(INC2,1,IN_PARALLEL);

	VoC_lw16_p(REG1,REG7,DEFAULT);
	VoC_lw16i_short(INC3,1,IN_PARALLEL);

//	VoC_shr16_ri(REG0,5,DEFAULT);
//	VoC_shr16_ri(REG1,5,IN_PARALLEL);

	// Spk Power
	VoC_jal(Coolsand_CmplxAppxNorm);

	//REG6:pSpkSigBufI
	//REG7:pSpkSigBufQ

	//ACC0:SpkDataNorm

	VoC_movreg16(REG1,ACC0_LO,DEFAULT);
	VoC_lw16i_short(FORMAT32,3,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG1,REG1,DEFAULT);
	
	VoC_lw16i(REG0,r_PF_SpkPowerBuf);

	VoC_lw16_d(REG1,g_ch);

	VoC_shr16_ri(REG1,-1,DEFAULT);
	VoC_shr32_ri(ACC0,-10,IN_PARALLEL);

	VoC_add16_rr(REG0,REG0,REG1,DEFAULT);
	VoC_shr32_ri(ACC0,10,IN_PARALLEL);

	VoC_lw16_d(REG1,m_AECProcPara_EchoUpdateFactor);

	VoC_lw32_p(ACC1,REG0,DEFAULT);//r_PF_SpkPowerBuf	
	
	VoC_movreg32(RL7,ACC0,DEFAULT);
	VoC_movreg32(REG23,REG67,IN_PARALLEL);//move 

//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	VoC_sub32_rr(REG45,RL7,ACC1,DEFAULT);

	VoC_shr32_ri(REG45,-10,DEFAULT);
	VoC_movreg32(ACC0,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG45,9,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

	VoC_mac32_rr(REG4,REG1,DEFAULT);

	VoC_macX_rr(REG5,REG1,DEFAULT);	

	VoC_NOP();

	//ACC0:pSpkPower

	VoC_shr32_ri(ACC0,-10,DEFAULT);
	VoC_lw16i_short(FORMAT32,5,IN_PARALLEL);


	VoC_lw32z(RL7,DEFAULT);//
	VoC_shr32_ri(ACC0,10,IN_PARALLEL);

	// Correlation between spk and err data

	//REG2:pSpkSigBufI
	//REG3:pSpkSigBufQ

	VoC_pop32(REG45,DEFAULT);
	VoC_movreg32(RL6,ACC0,IN_PARALLEL);//save *pSpkPower
	
	VoC_sw32_p(ACC0,REG0,DEFAULT);
	VoC_lw16i_short(REG0,0,IN_PARALLEL);

	//REG4:*pErrDataI
	//REG5:*pErrDataQ

	//	r_PF_CCErrSpkVecBufI
	//	r_PF_CCErrSpkVecBufQ

	VoC_lw16_d(REG7,g_ch);
	
	VoC_sub16_rr(REG5,REG0,REG5,DEFAULT);	
	VoC_shr16_ri(REG7,-4,IN_PARALLEL);//ch + FILTER_LEN -3+short to int -1

//	VoC_lw16i_short(FORMAT16,7-16,DEFAULT);
	
	VoC_lw16i_set_inc(REG0,r_PF_CCErrSpkVecBufI,2);
	VoC_lw16i_set_inc(REG1,r_PF_CCErrSpkVecBufQ,2);	
			
	VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
	
	VoC_lw16_d(REG7,m_AECProcPara_EchoUpdateFactor);


	//	for (i=0; i<FILTER_LEN; i++)
	VoC_loop_i(0,FILTER_LEN);
	
		// Corr with each Err and SpkBufData

		//	VoC_jal(Coolsand_CmplxMul);
		VoC_multf32_rp(ACC0,REG4,REG2,DEFAULT);

		VoC_multf32_rp(ACC1,REG5,REG3,DEFAULT);

		VoC_shr32_ri(ACC0,-10,DEFAULT);
		VoC_shr32_ri(ACC1,-10,DEFAULT);

		VoC_shr32_ri(ACC0,10,DEFAULT);
		VoC_shr32_ri(ACC1,10,IN_PARALLEL);

		VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);
		VoC_sub32_rr(RL6,ACC0,ACC1,IN_PARALLEL);//InstantCorrI

		VoC_multf32inc_rp(ACC1,REG5,REG2,DEFAULT);

		VoC_shr32_ri(ACC0,-10,DEFAULT);

		VoC_shr32_ri(ACC1,-10,DEFAULT);
		VoC_push32(REG45,IN_PARALLEL);

		VoC_shr32_ri(ACC0,10,DEFAULT);
		VoC_shr32_ri(ACC1,10,IN_PARALLEL);

		VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);//InstantCorrQ
		VoC_push32(REG23,IN_PARALLEL);

		VoC_shr32_ri(RL6,-10,DEFAULT);
		VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

		VoC_shr32_ri(RL6,10,DEFAULT);
		VoC_shr32_ri(ACC1,10,IN_PARALLEL);

		// Smoothing
		//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);
		//	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

//		VoC_movreg16(REG2,RL6_LO,DEFAULT);//InstantCorrI
//		VoC_movreg16(REG3,ACC1_LO,IN_PARALLEL);//InstantCorrQ

		VoC_sub32_rp(REG23,RL6,REG0,DEFAULT);
		VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

		VoC_sub32_rp(REG45,ACC1,REG1,DEFAULT);
		VoC_lw16i_short(FORMATX,-8,IN_PARALLEL);

		VoC_shr32_ri(REG23,-10,DEFAULT);
		VoC_shr32_ri(REG45,-10,IN_PARALLEL);

		VoC_shr32_ri(REG23,9,DEFAULT);
		VoC_shr32_ri(REG45,9,IN_PARALLEL);

		VoC_shru16_ri(REG2,1,DEFAULT);
		VoC_shru16_ri(REG4,1,IN_PARALLEL);
		
		VoC_multf32_rr(ACC0,REG2,REG7,DEFAULT);
		VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);

		VoC_macX_rr(REG3,REG7,DEFAULT);
		VoC_macX_rr(REG5,REG7,IN_PARALLEL);

		VoC_NOP();

		VoC_shr32_ri(ACC0,-10,DEFAULT);
		VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

		VoC_shr32_ri(ACC0,10,DEFAULT);
		VoC_shr32_ri(ACC1,10,IN_PARALLEL);

		VoC_add32_rp(ACC0,ACC0,REG0,DEFAULT);

		VoC_add32_rp(ACC1,ACC1,REG1,DEFAULT);

		VoC_shr32_ri(ACC0,-10,DEFAULT);
		VoC_shr32_ri(ACC1,-10,IN_PARALLEL);

		VoC_shr32_ri(ACC0,10,DEFAULT);
		VoC_shr32_ri(ACC1,10,IN_PARALLEL);

		VoC_NOP();

		VoC_sw32inc_p(ACC0,REG0,DEFAULT);
		VoC_shr32_ri(ACC0,5,IN_PARALLEL);

		VoC_sw32inc_p(ACC1,REG1,DEFAULT);
		VoC_shr32_ri(ACC1,5,IN_PARALLEL);

		// Corr Norm 	
	//	VoC_jal(Coolsand_CmplxAppxNorm_int);

		VoC_shr32_ri(ACC0,-17,DEFAULT);
		VoC_shr32_ri(ACC1,-17,IN_PARALLEL);

		VoC_shr32_ri(ACC0,17,DEFAULT);
		VoC_shr32_ri(ACC1,17,IN_PARALLEL);

		VoC_movreg32(REG45,ACC0,DEFAULT);
		VoC_shr32_ri(ACC0,31,IN_PARALLEL);

		VoC_xor32_rr(REG45,ACC0,DEFAULT);
		
		VoC_movreg32(RL6,ACC1,DEFAULT);
		VoC_shr32_ri(ACC1,31,IN_PARALLEL);

		VoC_xor32_rr(RL6,ACC1,DEFAULT);
		VoC_sub32_rr(ACC0,REG45,ACC0,IN_PARALLEL);

		VoC_sub32_rr(ACC1,RL6,ACC1,DEFAULT);

		VoC_shr32_ri(ACC0,-17,DEFAULT);
		VoC_shr32_ri(ACC1,-17,IN_PARALLEL);

		VoC_shr32_ri(ACC0,17,DEFAULT);
		VoC_shr32_ri(ACC1,17,IN_PARALLEL);


	   VoC_bgt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L0,ACC0,ACC1)
		 VoC_movreg32(ACC0,ACC1,DEFAULT);
		 VoC_movreg32(ACC1,ACC0,IN_PARALLEL);//swap
	Coolsand_EchoPSDEst_MFCOHFFix_L0:

		VoC_movreg32(RL6,ACC0,DEFAULT);
		VoC_movreg32(REG45,ACC1,IN_PARALLEL);
		
		VoC_shr32_ri(ACC0,-5,DEFAULT);//MaxVal*32
		VoC_shr32_ri(ACC1,-1,IN_PARALLEL);

 		VoC_sub32_rr(ACC0,ACC0,RL6,DEFAULT);//MaxVal*31 == MaxVal*32 - MaxVal 
		VoC_add32_rr(ACC1,ACC1,REG45,IN_PARALLEL);//MinVal*3 ==MinVal*2+MinVal

		VoC_shr32_ri(ACC0,5,DEFAULT); //(MaxVal*31)>>5
		VoC_shr32_ri(ACC1,3,IN_PARALLEL);
        
		VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

		VoC_shr32_ri(ACC0,-17,DEFAULT);			

		VoC_shr32_ri(ACC0,17,DEFAULT);
		VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
		
		VoC_movreg16(REG6,ACC0_LO,DEFAULT);
		VoC_movreg32(ACC0,RL7,IN_PARALLEL);
		// Sum Norm2
//		InstantNorm2 = RErrSpkNorm*RErrSpkNorm;
//		SumCorrNorm2 = SumCorrNorm2+InstantNorm2;

		VoC_mac32_rr(REG6,REG6,DEFAULT);
		
		VoC_pop32(REG23,DEFAULT);
		VoC_lw16i_short(FORMAT32,5,IN_PARALLEL);
		
		VoC_pop32(REG45,DEFAULT);
		VoC_movreg32(RL7,ACC0,IN_PARALLEL);	
			
	VoC_endloop0


	//RL7:SumCorrNorm2

	VoC_lw16i(REG0,r_PF_PrevEchoPowerEst);
	VoC_lw16i(REG1,r_PF_PrevSpkPower);
	VoC_lw16i(REG2,r_PF_SpkPowerBuf);


	VoC_lw16_d(REG4,g_ch);

	VoC_lw16i_short(REG3,2,DEFAULT);
	VoC_shr16_ri(REG4,-1,IN_PARALLEL);
	
	VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
	VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

	VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
	
	VoC_lw32_p(RL6,REG0,DEFAULT);
	VoC_lw32_p(ACC0,REG1,DEFAULT);	
	VoC_lw32_p(ACC1,REG2,DEFAULT);	

	VoC_lw16_d(REG4,r_PF_CL_State);


	VoC_bne16_rr(Coolsand_EchoPSDEst_MFCOHFFix_L4,REG3,REG4)	
	VoC_bez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L4,RL6)

		//VoC_jal(Coolsand_InstantSNR);

		VoC_push32(RL7,DEFAULT);//SumCorrNorm2Val

		VoC_shr32_ri(ACC0,4,DEFAULT);
		VoC_push32(RL6,IN_PARALLEL);

		VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L1,ACC0)
			VoC_lw16i_short(ACC0_LO,1,DEFAULT);
			VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
Coolsand_EchoPSDEst_MFCOHFFix_L1:
		VoC_movreg32(RL6,ACC0,DEFAULT);
		VoC_lw16i_short(REG2,18,IN_PARALLEL);
		//RL6:NoisePower_r.d
		//REG2:oisePower_r.t
		
		//REG3:NoisePowerFirstBitPos
		VoC_jal(Coolsand_MSBPos);

		VoC_lw16i_short(REG7,8,DEFAULT);
		VoC_lw32z(ACC0,IN_PARALLEL);
	
		//	int InstantSNRShiftVal = (NoisePower_r.f+1-NoisePowerFirstBitPos - pSigPower->f+pInstantSNR->f);
		VoC_sub16_rr(REG3,REG3,REG7,DEFAULT);
		VoC_lw16i_short(ACC0_LO,1,IN_PARALLEL);
	
		VoC_shr32_rr(ACC1,REG3,DEFAULT);
		VoC_shr32_ri(ACC0,-11,IN_PARALLEL);//2048
	
		VoC_shr32_ri(ACC1,-9,DEFAULT);
		VoC_lw16i_short(REG2,0,IN_PARALLEL);

		VoC_shr32_ri(ACC1,9,DEFAULT);
		VoC_pop32(REG01,IN_PARALLEL);//*pPrevEchoPowerEst

		VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L2,ACC1,ACC0)
			VoC_lw16i_short(REG2,8,DEFAULT);			
Coolsand_EchoPSDEst_MFCOHFFix_L2:

		VoC_lw16i_short(REG4,5,DEFAULT);

		VoC_shr32_rr(ACC1,REG2,DEFAULT);
		VoC_shr32_ri(REG01,6,IN_PARALLEL);

		VoC_shr32_ri(REG01,-16,DEFAULT);
		VoC_shr32_ri(ACC1,-16,IN_PARALLEL);

		VoC_movreg16(REG3,ACC1_HI,DEFAULT);
		VoC_sub16_rr(REG4,REG4,REG2,IN_PARALLEL);
		
		VoC_multf32_rr(REG01,REG1,REG3,DEFAULT);
		VoC_movreg16(FORMAT32,REG4,IN_PARALLEL);

		VoC_lw16_d(REG3,m_AECProcPara_PFDTEchoOverEstFactor)
		
		VoC_shr32_ri(REG01,-8,DEFAULT);
		VoC_lw16i_short(FORMAT32,1,IN_PARALLEL);

		VoC_shr32_ri(REG01,7,DEFAULT);
		VoC_lw16i_short(FORMATX,1-15,IN_PARALLEL);

		VoC_shru16_ri(REG0,1,DEFAULT);

		VoC_multf32_rr(ACC0,REG0,REG3,DEFAULT);

		VoC_macX_rr(REG1,REG3,DEFAULT);
		// Residual Echo Power
		VoC_pop32(RL7,IN_PARALLEL);//SumCorrNorm2Val


		VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);

		VoC_lw16_d(REG4,g_ch);

		VoC_shr32_ri(ACC0,-8,DEFAULT);
		VoC_shr16_ri(REG4,-1,IN_PARALLEL);

		VoC_shr32_ri(ACC0,8,DEFAULT);
		VoC_add16_rr(REG0,REG0,REG4,IN_PARALLEL);

		VoC_movreg32(ACC1,ACC0,DEFAULT);
		
		VoC_lw32_p(RL6,REG0,DEFAULT);//pEchoPowerEst->d
		VoC_shr32_ri(ACC1,-2,IN_PARALLEL);

		VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L3,RL6,ACC1)
			VoC_shr32_ri(RL6,2,DEFAULT);
			VoC_movreg32(ACC0,RL6,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L3:


		VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L7,ACC0)
			VoC_lw16i_short(ACC0_LO,1,DEFAULT);
			VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
		
			VoC_jump(Coolsand_EchoPSDEst_MFCOHFFix_L7);

Coolsand_EchoPSDEst_MFCOHFFix_L4:


//	VoC_pop32(RL6,DEFAULT);
	VoC_movreg32(RL6,ACC1,DEFAULT);
	//RL6:pSpkPower->d

	VoC_lw16i_short(ACC1_LO,4,DEFAULT);
	VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

	VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L5,RL6,ACC1);
	VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L5,RL7);
		VoC_lw16i_short(RL7_HI,0,DEFAULT);
		VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);	
Coolsand_EchoPSDEst_MFCOHFFix_L5:
	

	VoC_bnez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L6,RL6);
	VoC_bez16_d(Coolsand_EchoPSDEst_MFCOHFFix_L6,r_PF_CL_State);
		VoC_lw32z(RL7,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L6:

	VoC_lw16i_short(REG2,22,DEFAULT);
	VoC_movreg32(ACC1,RL6,IN_PARALLEL);//pSpkPower->d

	VoC_jal(Coolsand_MSBPos);	

	//REG3:SpkPowerFirstBitPos
	//RL7:SumCorrNorm2
	//ACC1:pSpkPower->d

	VoC_lw16i_short(REG4,11,DEFAULT);
//	VoC_pop32(ACC0,IN_PARALLEL);//r_PF_EchoPowerEstBuf

	VoC_lw16i(REG0,r_PF_EchoPowerEstBuf);
	VoC_lw16_d(REG7,g_ch);

	VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
	VoC_shr16_ri(REG7,-1,IN_PARALLEL);

	VoC_shr32_rr(RL7,REG3,DEFAULT);
	VoC_add16_rr(REG0,REG0,REG7,IN_PARALLEL);

	VoC_shr32_ri(RL7,-8,DEFAULT);
	VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

	VoC_shr32_ri(RL7,8,DEFAULT);
	VoC_lw16i_short(ACC0_LO,1,IN_PARALLEL);

	VoC_bez32_r(Coolsand_EchoPSDEst_MFCOHFFix_L7,RL7)

		VoC_movreg32(ACC0,RL7,DEFAULT);	
		VoC_movreg32(ACC1,RL7,IN_PARALLEL);

		VoC_shr32_ri(ACC1,-2,DEFAULT);
		VoC_lw32_p(RL6,REG0,IN_PARALLEL);//pEchoPowerEst->d

		VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_L7,RL6,ACC1)
			VoC_shr32_ri(RL6,2,DEFAULT);
			VoC_movreg32(ACC0,RL6,DEFAULT);
Coolsand_EchoPSDEst_MFCOHFFix_L7:


	// Record Prev

	VoC_lw16_d(REG7,g_ch);

	VoC_sw32_p(ACC0,REG0,DEFAULT);//pEchoPowerEst
	VoC_shr16_ri(REG7,-1,IN_PARALLEL);

	VoC_lw16i(REG1,r_PF_SpkPowerBuf);
	VoC_lw16i(REG2,r_PF_PrevSpkPower);
	VoC_lw16i(REG3,r_PF_PrevEchoPowerEst);

	VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
	VoC_add16_rr(REG2,REG2,REG7,IN_PARALLEL);

	VoC_add16_rr(REG3,REG3,REG7,DEFAULT);

	VoC_lw32_d(RL6,m_AECProcPara_PFPrevEchoEstLimit);
	
	VoC_lw32_p(ACC1,REG1,DEFAULT);
	VoC_shr32_ri(RL6,-4,IN_PARALLEL);


	VoC_bnez16_d(Coolsand_EchoPSDEst_MFCOHFFix_End,r_PF_CL_State);
	VoC_bngt32_rr(Coolsand_EchoPSDEst_MFCOHFFix_End,ACC1,RL6);
	VoC_bngt32_rd(Coolsand_EchoPSDEst_MFCOHFFix_End,ACC0,m_AECProcPara_PFPrevEchoEstLimit);

		VoC_sw32_p(ACC1,REG2,DEFAULT);//		*pPrevSpkPower = *pSpkPower;
		VoC_sw32_p(ACC0,REG3,DEFAULT);

Coolsand_EchoPSDEst_MFCOHFFix_End:

//	r_PF_CL_State//State

//	r_PF_SpkPowerBuf//pSpkPower

//	r_PF_EchoPowerEstBuf//pEchoPowerEst

//	m_AECProcPara_PFPrevEchoEstLimit//pPrevEchoEstLimit


	VoC_pop16(RA,DEFAULT);
	VoC_NOP();	
	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_ControlLogicDTD
 *
 * Description: 
 *
 * Inputs:  : 
 *			:
 *			:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/24/2011
 **************************************************************************************/
void Coolsand_ControlLogicDTD(void)
{
	VoC_push16(RA,DEFAULT);

	// Instant Mic Power
	//	InstantPower(pMicDataI, pMicDataQ, &InstantMicPower);

	VoC_lw16i(REG0,g_MicAnaFilterOutBufI);
	VoC_lw16i(REG1,g_MicAnaFilterOutBufQ);

	VoC_add16_rd(REG0,REG0,CLChnl);
	VoC_add16_rd(REG1,REG1,CLChnl);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG1,DEFAULT);

	VoC_jal(Cooland_InstantPower);

	VoC_movreg32(RL7,ACC0,DEFAULT);

	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);


	VoC_lw16i(REG1,r_PF_CL_DTDMicSmoothPower);

	VoC_add16_rd(REG1,REG1,g_DTD_ch);

	VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

	VoC_lw32_p(ACC0,REG1,DEFAULT);



	// Smooth Mic Power
	//	*pMicPower = FirstOrderIIRSmooth2(pMicPower, &InstantMicPower, pUpdateFactor);
	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	//	if (pMicPower->d == 0)
	//		pMicPower->d = 1;

	VoC_bnez32_r(Coolsand_ControlLogicDTD_L0,ACC0)

		VoC_lw16i_short(ACC0_LO,1,DEFAULT);
		VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L0:
	// Instant Err Power
	//	InstantPower(pErrDataI, pErrDataQ, &InstantErrPower);


	VoC_lw16i(REG0,g_AF_ErrDataBufI);
	VoC_lw16i(REG1,g_AF_ErrDataBufQ);

	VoC_add16_rd(REG0,REG0,CLChnl);
	VoC_add16_rd(REG1,REG1,CLChnl);

	VoC_lw16_p(REG0,REG0,DEFAULT);
	VoC_lw16_p(REG1,REG1,DEFAULT);


	VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_NOP();

	VoC_sw32_p(ACC0,REG2,DEFAULT);




	VoC_jal(Cooland_InstantPower);


	// Smooth Err Power
	//	*pErrPower = FirstOrderIIRSmooth2(pErrPower, &InstantErrPower, pUpdateFactor);

	VoC_movreg32(RL7,ACC0,DEFAULT);

	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);


	VoC_lw16i(REG1,r_PF_CL_DTDErrSmoothPower);

	VoC_add16_rd(REG1,REG1,g_DTD_ch);

	VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

	VoC_lw32_p(ACC0,REG1,DEFAULT);


	VoC_jal(Coolsand_FirstOrderIIRSmooth2);
	//	if (pErrPower->d == 0)
	//		pErrPower->d = 1;
	VoC_bnez32_r(Coolsand_ControlLogicDTD_L1,ACC0)

		VoC_lw16i_short(ACC0_LO,1,DEFAULT);
		VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L1:

	// Instant Echo Power
	//	InstantPower(&EchoDataI, &ConjEchoDataQ, &InstantEchoPower);

	VoC_lw16i(REG0,g_MicAnaFilterOutBufI);
	VoC_lw16i(REG1,g_MicAnaFilterOutBufQ);

	VoC_add16_rd(REG0,REG0,CLChnl);
	VoC_add16_rd(REG1,REG1,CLChnl);

	VoC_lw16_p(REG0,REG0,DEFAULT);

	VoC_lw16_p(REG1,REG1,DEFAULT);


	VoC_lw16i(REG2,g_AF_ErrDataBufI);
	VoC_lw16i(REG3,g_AF_ErrDataBufQ);

	VoC_add16_rd(REG2,REG2,CLChnl);
	VoC_add16_rd(REG3,REG3,CLChnl);

	VoC_lw16_p(REG2,REG2,DEFAULT);

	VoC_lw16_p(REG3,REG3,DEFAULT);

	VoC_sub16_rr(REG0,REG0,REG2,DEFAULT);
	VoC_sub16_rr(REG1,REG3,REG1,IN_PARALLEL);


	VoC_lw16i(REG2,r_PF_CL_DTDErrSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_push32(REG01,DEFAULT);//EchoDataI and ConjEchoDataQ

	VoC_sw32_p(ACC0,REG2,DEFAULT);


	VoC_jal(Cooland_InstantPower);


	// Smooth Echo Power
	//	*pEchoPower = FirstOrderIIRSmooth2(pEchoPower, &InstantEchoPower, pUpdateFactor);

	VoC_movreg32(RL7,ACC0,DEFAULT);

	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

	VoC_lw16i(REG1,r_PF_CL_DTDEchoSmoothPower);

	VoC_add16_rd(REG1,REG1,g_DTD_ch);

	VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

	VoC_lw32_p(ACC0,REG1,DEFAULT);

	VoC_jal(Coolsand_FirstOrderIIRSmooth2);

	//	if (pEchoPower->d == 0)
	//		pEchoPower->d = 1;
	VoC_bnez32_r(Coolsand_ControlLogicDTD_L2,ACC0)

		VoC_lw16i_short(ACC0_LO,1,DEFAULT);
		VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

Coolsand_ControlLogicDTD_L2:
		
	// Instant Mic SNR
	//	InstantSNR(pMicNoise, pMicPower, &InstantMicSNR);


	VoC_lw16i(REG2,r_PF_CL_DTDEchoSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_NOP();

	VoC_sw32_p(ACC0,REG2,DEFAULT);



	VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_NOP();

	VoC_lw32_p(ACC1,REG2,DEFAULT);



	VoC_lw16i(REG2,r_PF_CL_DTDMicNoisePower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_NOP();

	VoC_lw32_p(ACC0,REG2,DEFAULT);


	VoC_jal(Coolsand_InstantSNR);

	//Mic Noise Power Est
	//	OverallNoiseEst(	pMicNoise, 
					//pMicPower, 
					//&PrevMicPower, 
					//&InstantMicSNR, 
					//*pUpdateFactor,
					//pMicNoiseFloorDevCnt,
					//FrameCnt);
	VoC_lw16i_short(REG7,8,DEFAULT);
	VoC_lw16i_short(REG2,7,IN_PARALLEL);
	VoC_lw16i(REG0,r_PF_CL_DTDMicSmoothPower);
	VoC_lw16i(REG1,r_PF_CL_DTDMicNoisePower);
	VoC_add16_rd(REG0,REG0,g_DTD_ch);
	VoC_add16_rd(REG1,REG1,g_DTD_ch);
		
	VoC_sw16_d(REG7,pInstantSNR_t);
	VoC_sw16_d(REG2,pInstantSNR_f);

	VoC_lw16_d(REG6,m_AF_FrameCnt);
	VoC_lw16i(REG7,r_PF_CL_DTDMicNoiseFloorDevCnt);

	VoC_add16_rd(REG7,REG7,g_DTD_ch);


	VoC_lw16_d(REG2,m_AECProcPara_PFCLDTDUpdateFactor);




	VoC_jal(Coolsand_OverallNoiseEst);


	// Instant XCorr Mic Err
	// CmplxMul(pMicDataI, pMicDataQ, pErrDataI, &ConjErrDataQ, &InstantXcorrI, &InstantXcorrQ);

	VoC_lw16i_short(REG0,0,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);


	VoC_lw16i(REG2,g_MicAnaFilterOutBufI);
	VoC_lw16i(REG3,g_MicAnaFilterOutBufQ);

	VoC_add16_rd(REG2,REG2,CLChnl);
	VoC_add16_rd(REG3,REG3,CLChnl);

	VoC_lw16_p(REG2,REG2,DEFAULT);

	VoC_lw16_p(REG3,REG3,DEFAULT);


	VoC_lw16i(REG4,g_AF_ErrDataBufI);
	VoC_lw16i(REG5,g_AF_ErrDataBufQ);

	VoC_add16_rd(REG4,REG4,CLChnl);
	VoC_add16_rd(REG5,REG5,CLChnl);

	VoC_lw16_p(REG4,REG4,DEFAULT);

	VoC_lw16_p(REG5,REG5,DEFAULT);

	VoC_sub16_rr(REG5,REG0,REG5,DEFAULT);	

	VoC_jal(Coolsand_CmplxMul);

	//ACC0:InstantXcorrI
	//ACC1:InstantXcorrQ

	// Smooth XCorr
	//*pXorrMicErrI = FirstOrderIIRSmooth2(pXorrMicErrI, &InstantXcorrI, pUpdateFactor);
	//*pXorrMicErrQ = FirstOrderIIRSmooth2(pXorrMicErrQ, &InstantXcorrQ, pUpdateFactor);

	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

	VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

	VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_movreg32(RL7,ACC1,IN_PARALLEL);


	VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicErrI);
	VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicErrQ);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);
	VoC_add16_rd(REG3,REG3,g_DTD_ch);

	VoC_lw32_p(ACC0,REG2,DEFAULT);

	VoC_lw32_p(ACC1,REG3,DEFAULT);



	VoC_jal(Coolsand_Double_FirstOrderIIRSmooth2);

	// Instant XCorr Mic Echo
	//	CmplxMul(pMicDataI, pMicDataQ, &EchoDataI, &ConjEchoDataQ, &InstantXEcorrI, &InstantXEcorrQ);

	VoC_pop32(REG45,DEFAULT);////EchoDataI and ConjEchoDataQ
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicErrI);
	VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicErrQ);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);
	VoC_add16_rd(REG3,REG3,g_DTD_ch);

	VoC_sw32_p(ACC0,REG2,DEFAULT);

	VoC_sw32_p(ACC1,REG3,DEFAULT);

	VoC_lw16i(REG2,g_MicAnaFilterOutBufI);
	VoC_lw16i(REG3,g_MicAnaFilterOutBufQ);

	VoC_add16_rd(REG2,REG2,CLChnl);
	VoC_add16_rd(REG3,REG3,CLChnl);

	VoC_lw16_p(REG2,REG2,DEFAULT);

	VoC_lw16_p(REG3,REG3,DEFAULT);
	
	VoC_jal(Coolsand_CmplxMul);

	//ACC0:InstantXcorrI
	//ACC1:InstantXcorrQ

	// Smooth XCorr
	//*pXorrMicEchoI = FirstOrderIIRSmooth2(pXorrMicEchoI, &InstantXEcorrI, pUpdateFactor);
	//*pXorrMicEchoQ = FirstOrderIIRSmooth2(pXorrMicEchoQ, &InstantXEcorrQ, pUpdateFactor);

	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

	VoC_lw16_d(REG1,m_AECProcPara_PFCLDTDUpdateFactor);

	VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_movreg32(RL7,ACC1,IN_PARALLEL);



	VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicEchoI);
	VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicEchoQ);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);
	VoC_add16_rd(REG3,REG3,g_DTD_ch);

	VoC_lw32_p(ACC0,REG2,DEFAULT);

	VoC_lw32_p(ACC1,REG3,DEFAULT);

	VoC_jal(Coolsand_Double_FirstOrderIIRSmooth2);

	VoC_lw16i(REG2,r_PF_CL_DTDXcorrMicEchoI);
	VoC_lw16i(REG3,r_PF_CL_DTDXcorrMicEchoQ);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);
	VoC_add16_rd(REG3,REG3,g_DTD_ch);

	VoC_sw32_p(ACC0,REG2,DEFAULT);

	VoC_sw32_p(ACC1,REG3,DEFAULT);


	// Decision: >T0
	//	CmplxAppxNorm(pXorrMicEchoI, pXorrMicEchoQ, &NormXcorrMicEcho);
	VoC_jal(Coolsand_CmplxAppxNorm_int);


	VoC_shr32_ri(ACC0,-8,DEFAULT);
	VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);


	VoC_shr32_ri(ACC0,8,DEFAULT);
	VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

	VoC_shr32_ri(ACC1,-10,DEFAULT);
	VoC_lw16i_short(REG7,8,IN_PARALLEL);	

	VoC_bgt32_rr(Coolsand_ControlLogicDTD_L3,ACC0,ACC1)
		VoC_lw16i_short(REG7,0,DEFAULT);	
Coolsand_ControlLogicDTD_L3:


	VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

//	VoC_NOP();

	VoC_lw16i(REG3,r_PF_CL_DTDMicNoisePower);

	VoC_add16_rd(REG3,REG3,g_DTD_ch);

	VoC_lw32_p(RL6,REG2,DEFAULT);

	VoC_sub32_rp(REG23,RL6,REG3,DEFAULT);


	VoC_shr32_rr(ACC0,REG7,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_movreg16(REG6,ACC0_LO,DEFAULT);	
	VoC_shr32_ri(REG23,-1,IN_PARALLEL);

	VoC_multf32_rr(RL6,REG6,REG6,DEFAULT); 
	VoC_shru16_ri(REG2,1,IN_PARALLEL);

	VoC_lw16_d(REG0,m_AECProcPara_PFCLDTDThreshold);

	VoC_lw16i_short(REG6,7,DEFAULT);
	VoC_lw16i_short(REG1,8,IN_PARALLEL);

	VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
	VoC_sub16_rr(REG1,REG7,REG1,IN_PARALLEL);

	VoC_lw16i(REG4,r_PF_CL_DTDEchoSmoothPower);

	VoC_add16_rd(REG4,REG4,g_DTD_ch);

	VoC_NOP();

	VoC_lw32_p(REG45,REG4,DEFAULT);

	VoC_movreg16(FORMAT32,REG6,DEFAULT);
	VoC_shr32_ri(REG45,-1,IN_PARALLEL);

	VoC_movreg16(FORMATX,REG1,DEFAULT);
	VoC_shru16_ri(REG4,1,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT); 
	VoC_multf32_rr(ACC1,REG4,REG0,IN_PARALLEL); 

	VoC_macX_rr(REG3,REG0,DEFAULT); 	
	VoC_macX_rr(REG5,REG0,IN_PARALLEL); 

	VoC_lw16i_short(FORMAT32,0,DEFAULT);

	VoC_movreg32(REG23,ACC0,DEFAULT);
	VoC_movreg32(REG45,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG23,-16,DEFAULT);
	VoC_shr32_ri(REG45,-16,IN_PARALLEL);
		
	VoC_bgtz16_r(Coolsand_ControlLogicDTD_L4,REG3)
		VoC_lw16i_short(REG3,1,DEFAULT);		
Coolsand_ControlLogicDTD_L4:

	VoC_bnez16_r(Coolsand_ControlLogicDTD_L5,REG5)
		VoC_lw16i_short(REG5,1,DEFAULT);		
Coolsand_ControlLogicDTD_L5:

	VoC_lw16i_short(REG6,23,DEFAULT);
	VoC_lw16i_short(FORMATX,7-15,IN_PARALLEL);

	VoC_multf32_rr(REG45,REG3,REG5,DEFAULT);
	VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

	VoC_lw16i_short(REG7,25,DEFAULT);
	VoC_shr16_ri(REG6,-1,IN_PARALLEL);

	VoC_shr32_ri(REG45,-1,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);

	VoC_shru16_ri(REG4,1,DEFAULT);
	VoC_push32(REG45,IN_PARALLEL);	

	VoC_lw16_d(REG2,m_AECProcPara__PFCLDTDThrRatio);

	VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);

	VoC_macX_rr(REG5,REG2,DEFAULT);

	VoC_NOP();

	VoC_movreg32(REG45,ACC0,DEFAULT);

	VoC_shr32_ri(REG45,-1,DEFAULT);

	VoC_shru16_ri(REG4,1,DEFAULT);

	VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);

	VoC_macX_rr(REG5,REG2,DEFAULT);

	VoC_NOP();
	
	VoC_sub32_rr(RL7,RL6,ACC0,DEFAULT);
	VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

	VoC_shr32_rr(RL7,REG6,DEFAULT);
	VoC_push32(RL6,IN_PARALLEL);

	VoC_lw16i(REG4,r_PF_CL_DTDXcorrMicErrI);
	VoC_lw16i(REG5,r_PF_CL_DTDXcorrMicErrQ);
	VoC_lw16i(REG7,r_PF_CL_DTDDecision);

	VoC_add16_rd(REG4,REG4,g_DTD_ch);
	VoC_add16_rd(REG5,REG5,g_DTD_ch);
	VoC_add16_rd(REG7,REG7,g_DTD_ch);

	VoC_lw32_p(ACC0,REG4,DEFAULT);
	VoC_lw32_p(ACC1,REG5,DEFAULT);
	VoC_sw32_p(RL7,REG7,DEFAULT);
	
	// Decision: >T1
	//		CmplxAppxNorm(pXorrMicErrI, pXorrMicErrQ, &NormXcorrMicErr);
	VoC_jal(Coolsand_CmplxAppxNorm_int);


	VoC_shr32_ri(ACC0,-8,DEFAULT);
	VoC_lw16i_short(ACC1_LO,1,IN_PARALLEL);


	VoC_shr32_ri(ACC0,8,DEFAULT);
	VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

	VoC_shr32_ri(ACC1,-10,DEFAULT);
	VoC_lw16i_short(REG7,8,IN_PARALLEL);	

	VoC_bgt32_rr(Coolsand_ControlLogicDTD_L6,ACC0,ACC1)
		VoC_lw16i_short(REG7,0,DEFAULT);	
Coolsand_ControlLogicDTD_L6:

	
	VoC_lw16i(REG2,r_PF_CL_DTDMicSmoothPower);

	VoC_add16_rd(REG2,REG2,g_DTD_ch);

	VoC_shr32_rr(ACC0,REG7,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

	VoC_lw32_p(REG23,REG2,DEFAULT);


	VoC_movreg16(REG6,ACC0_LO,DEFAULT);	
	VoC_shr32_ri(REG23,-1,IN_PARALLEL);

	VoC_multf32_rr(RL6,REG6,REG6,DEFAULT); 
	VoC_shru16_ri(REG2,1,IN_PARALLEL);

	VoC_lw16_d(REG0,m_AECProcPara_PFCLDTD2ndThreshold);

	VoC_lw16i_short(REG6,7,DEFAULT);
	VoC_lw16i_short(REG1,8,IN_PARALLEL);

	VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
	VoC_sub16_rr(REG1,REG7,REG1,IN_PARALLEL);


	VoC_lw16i(REG4,r_PF_CL_DTDErrSmoothPower);
	
	VoC_add16_rd(REG4,REG4,g_DTD_ch);
	
	VoC_NOP();
	
	VoC_lw32_p(REG45,REG4,DEFAULT);


	VoC_movreg16(FORMAT32,REG6,DEFAULT);
	VoC_shr32_ri(REG45,-1,IN_PARALLEL);

	VoC_movreg16(FORMATX,REG1,DEFAULT);
	VoC_shru16_ri(REG4,1,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG2,REG0,DEFAULT); 
	VoC_multf32_rr(ACC1,REG4,REG0,IN_PARALLEL); 

	VoC_macX_rr(REG3,REG0,DEFAULT); 	
	VoC_macX_rr(REG5,REG0,IN_PARALLEL); 

	VoC_lw16i_short(FORMAT32,0,DEFAULT);

	VoC_movreg32(REG23,ACC0,DEFAULT);
	VoC_movreg32(REG45,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG23,-16,DEFAULT);
	VoC_shr32_ri(REG45,-16,IN_PARALLEL);
		
	VoC_bnez16_r(Coolsand_ControlLogicDTD_L7,REG3)
		VoC_lw16i_short(REG3,1,DEFAULT);		
Coolsand_ControlLogicDTD_L7:

	VoC_bnez16_r(Coolsand_ControlLogicDTD_L8,REG5)
		VoC_lw16i_short(REG5,1,DEFAULT);		
Coolsand_ControlLogicDTD_L8:

	VoC_lw16i_short(REG6,23,DEFAULT);

	VoC_multf32_rr(ACC0,REG3,REG5,DEFAULT);
	VoC_sub16_rr(REG6,REG6,REG7,IN_PARALLEL);

	VoC_lw16i_short(REG5,25,DEFAULT);
	VoC_shr16_ri(REG6,-1,IN_PARALLEL);
	
	VoC_sub32_rr(ACC1,RL6,ACC0,DEFAULT);
	VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);

	VoC_shr32_rr(ACC1,REG6,DEFAULT);
	VoC_pop32(RL7,IN_PARALLEL);
	
	VoC_lw16i(REG6,r_PF_CL_DTD2ndDecision);

	VoC_add16_rd(REG6,REG6,g_DTD_ch);

	VoC_lw16i_short(REG0,0,DEFAULT);
	VoC_pop32(ACC1,IN_PARALLEL);

	VoC_sw32_p(ACC1,REG6,DEFAULT);

	VoC_bngt32_rr(Coolsand_ControlLogicDTD_L9,ACC1,RL7)
	VoC_bngt32_rr(Coolsand_ControlLogicDTD_L9,RL6,ACC0)
		VoC_lw16i_short(REG0,1,DEFAULT);
Coolsand_ControlLogicDTD_L9:
	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_CmplxMul
 *
 * Description: 
 *
 * Inputs:  : 
 *			:
 *			:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/27/2011
 **************************************************************************************/
void Coolsand_CmplxMul(void)
{

	VoC_multf32_rr(ACC0,REG4,REG2,DEFAULT);
	VoC_multf32_rr(ACC1,REG5,REG3,IN_PARALLEL);

	VoC_multf32_rr(ACC0,REG4,REG3,DEFAULT);
	VoC_multf32_rr(ACC1,REG5,REG2,IN_PARALLEL);

	VoC_movreg32(RL6,ACC0,DEFAULT);
	VoC_movreg32(RL7,ACC1,IN_PARALLEL);

	VoC_shr32_ri(ACC0,-8,DEFAULT);
	VoC_shr32_ri(ACC1,-8,IN_PARALLEL);

	VoC_shr32_ri(ACC0,8,DEFAULT);
	VoC_shr32_ri(ACC1,8,IN_PARALLEL);

	VoC_shr32_ri(RL6,-8,DEFAULT);
	VoC_shr32_ri(RL7,-8,IN_PARALLEL);

	VoC_shr32_ri(RL6,8,DEFAULT);
	VoC_shr32_ri(RL7,8,IN_PARALLEL);

	VoC_sub32_rr(ACC0,RL6,RL7,DEFAULT);//InstantCorrI
	VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//InstantCorrQ

	VoC_shr32_ri(ACC0,-8,DEFAULT);
	VoC_shr32_ri(ACC1,-8,IN_PARALLEL);

	VoC_shr32_ri(ACC0,8,DEFAULT);
	VoC_shr32_ri(ACC1,8,IN_PARALLEL);

	VoC_return;


}

/**************************************************************************************
 * Function:    Coolsand_Double_FirstOrderIIRSmooth2
 *
 * Description: 
 *
 * Inputs:  : 
 *			:
 *			:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       10/27/2011
 **************************************************************************************/
void Coolsand_Double_FirstOrderIIRSmooth2(void)
{
	VoC_sub32_rr(REG23,RL6,ACC0,DEFAULT);
	VoC_sub32_rr(REG45,RL7,ACC1,IN_PARALLEL);

	VoC_shr32_ri(REG23,-8,DEFAULT);
	VoC_shr32_ri(REG45,-8,DEFAULT);

	VoC_shr32_ri(REG23,7,DEFAULT);
	VoC_shr32_ri(REG45,7,IN_PARALLEL);

	VoC_shru16_ri(REG2,1,DEFAULT);
	VoC_shru16_ri(REG4,1,IN_PARALLEL);

	VoC_mac32_rr(REG2,REG1,DEFAULT);
	VoC_mac32_rr(REG4,REG1,IN_PARALLEL);

	VoC_macX_rr(REG3,REG1,DEFAULT);	
	VoC_macX_rr(REG5,REG1,IN_PARALLEL);	
		
	VoC_return;
}

/**************************************************************************************
 * Function:    RDA_Pitch_Filter
 *
 * Description: 
 *
 * Inputs:  : 
 *			:
 *			:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       1/17/2014
 **************************************************************************************/
/*
void RDA_Pitch_Filter(void)
{
    VoC_push16(RA,DEFAULT);
	
	//mic data  will delay 154 sameples after aec processing;
	
    VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_ADDR+154,1);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_loop_i(0,160)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
        VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0
	
	VoC_lw16i_short(REG5,0,DEFAULT);//init Pitch_filter_count;
PitchFilterBegain:
	//memcpy(speakerBuf_history,speakerBuf_history+40,40*2*5);
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+40,1);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR,1);
	VoC_sw16_d(REG5,PITCH_FILTER_COUNT);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_loop_i(0,200)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0
		
	//memcpy(speakerBuf_history+40*5,speakerBuf+40*i,40*2);
	VoC_lw16i(REG3,40);
	VoC_mult16_rd(REG3,REG3,PITCH_FILTER_COUNT);
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR+200,1);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_lw32z(ACC0,IN_PARALLEL);
	VoC_loop_i(0,40)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0


	  //Step 1:
//	    r1_max=0;
//		r2_max=0;
//		r3_max=0;
//
//		r1_index=0;
//		r2_index=0;
// 		r3_index=0;

	VoC_sw32_d(ACC0,PITCH_FILTER_R1_MAX);
	VoC_sw32_d(ACC0,PITCH_FILTER_R2_MAX);
	VoC_sw32_d(ACC0,PITCH_FILTER_R3_MAX);
	VoC_sw16_d(ACC0_LO,PITCH_FILTER_R1_INDEX);
	VoC_sw16_d(ACC0_LO,PITCH_FILTER_R2_INDEX);
	VoC_sw16_d(ACC0_LO,PITCH_FILTER_R3_INDEX);
      

//	for (j=80;j<=147;j++)
//	{
//		r_temp=0;
//	
//		for (k=0;k<80;k++)
//		{
//			r_temp+=(speakerBuf_history[k+160]*speakerBuf_history[k+160-j])>>7;
//		}
//	
//		if (r_temp>r1_max)
//		{
//			r1_max=r_temp;
//			r1_index=j;
//		}				
//	
// 	}

	VoC_lw16i_set_inc(REG2,80,1);
    VoC_loop_i(0,68)//147 - 80 + 1= 68;
        VoC_lw32z(ACC0,DEFAULT);
		VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
	    VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	    VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
		VoC_sub16_rr(REG1,REG1,REG2,DEFAULT);
		VoC_loop_i(1,80)
			VoC_lw16inc_p(REG3,REG0,DEFAULT);
			VoC_mac32inc_rp(REG3,REG1,DEFAULT);
	    VoC_endloop1
		VoC_NOP();
		VoC_bngt32_rd(PitchFilterR1Max,ACC0,PITCH_FILTER_R1_MAX);
		VoC_sw32_d(ACC0,PITCH_FILTER_R1_MAX);
		VoC_sw16_d(REG2,PITCH_FILTER_R1_INDEX);
PitchFilterR1Max:
        VoC_inc_p(REG2,DEFAULT);
	VoC_endloop0

//	for (j=40;j<=79;j++)
//	{
//		r_temp=0;
//		for (k=0;k<80;k++)
//		{
//			r_temp+=(speakerBuf_history[k+160]*speakerBuf_history[k+160-j])>>7;
//		}
//	
//		if (r_temp>r2_max)
//		{
//			r2_max=r_temp;
//			r2_index=j;
//		}				
//	
// 	}
	VoC_lw16i_set_inc(REG2,40,1);
	VoC_loop_i(0,40)//79 - 40 + 1= 39;
		VoC_lw32z(ACC0,DEFAULT);
	   VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
		VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
		VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
		VoC_sub16_rr(REG1,REG1,REG2,DEFAULT);
		VoC_loop_i(1,80)
			VoC_lw16inc_p(REG3,REG0,DEFAULT);
			VoC_mac32inc_rp(REG3,REG1,DEFAULT);
		VoC_endloop1
		VoC_NOP();
		VoC_bngt32_rd(PitchFilterR2Max,ACC0,PITCH_FILTER_R2_MAX);
		VoC_sw32_d(ACC0,PITCH_FILTER_R2_MAX);
		VoC_sw16_d(REG2,PITCH_FILTER_R2_INDEX);
PitchFilterR2Max:
		VoC_inc_p(REG2,DEFAULT);
	VoC_endloop0
		
//	for (j=20;j<=39;j++)
//	{
//		r_temp=0;
//	
//		for (k=0;k<80;k++)
//		{
//			r_temp+=(speakerBuf_history[k+160]*speakerBuf_history[k+160-j])>>7;
//		}
//	
//		if (r_temp>r3_max)
//		{
//			r3_max=r_temp;
//			r3_index=j;
//		}				
//	
// 	}

    VoC_lw16i_set_inc(REG2,20,1);
	VoC_loop_i(0,20)//39 - 20 + 1= 20;
		VoC_lw32z(ACC0,DEFAULT);
	    VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
		VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
		VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
		VoC_sub16_rr(REG1,REG1,REG2,DEFAULT);
		VoC_loop_i(1,80)
			VoC_lw16inc_p(REG3,REG0,DEFAULT);
			VoC_mac32inc_rp(REG3,REG1,DEFAULT);
		VoC_endloop1
		VoC_NOP();
		VoC_bngt32_rd(PitchFilterR3Max,ACC0,PITCH_FILTER_R3_MAX);
		VoC_sw32_d(ACC0,PITCH_FILTER_R3_MAX);
		VoC_sw16_d(REG2,PITCH_FILTER_R3_INDEX);
PitchFilterR3Max:
		VoC_inc_p(REG2,DEFAULT);
	VoC_endloop0

    
	//Step 2:
//	r_temp1=0;
//	for (k=0;k<80;k++)
//	{
//		r_temp1+=(speakerBuf_history[k+160-r1_index]*speakerBuf_history[k+160-r1_index])>>7;
//	}
//	//	r_temp1=sqrt(r_temp1);
//
//	r_temp1=r_temp1>>B_Qn;
//
//	if (r_temp1!=0) 
//	{
//		r1_norm=r1_max/r_temp1;
//	}
//	else
//	{
//		//r1_norm=0x7fffffff;
//		r1_norm=0;
//	}

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);	
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	VoC_sub16_rd(REG0,REG0,PITCH_FILTER_R1_INDEX);
	VoC_loop_i(1,80)
		VoC_lw16inc_p(REG3,REG0,DEFAULT);
		VoC_mac32_rr(REG3,REG3,DEFAULT);
	VoC_endloop1
    VoC_NOP();
	VoC_shr32_ri(ACC0,10,DEFAULT);
	VoC_lw32z(REG67,IN_PARALLEL);
	VoC_bez32_r(PitchFilterR1NorZero,ACC0);
	VoC_lw32_d(ACC1,PITCH_FILTER_R1_MAX);
	VoC_jal(RDA_DIV32_S);
PitchFilterR1NorZero:
    VoC_sw32_d(REG67,PITCH_FILTER_R1_NORM);

//	r_temp2=0;
//	for (k=0;k<80;k++)
//	{
//		r_temp2+=(speakerBuf_history[k+160-r2_index]*speakerBuf_history[k+160-r2_index])>>7;
//	}
////	r_temp2=sqrt(r_temp2);
//
//	r_temp2=r_temp2>>B_Qn;
//	if (r_temp2!=0) 
//	{
//		r2_norm=r2_max/r_temp2;
//	}
//	else
//	{
//		//r2_norm=0x7fffffff;
//		r2_norm=0;
// 	}

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	VoC_sub16_rd(REG0,REG0,PITCH_FILTER_R2_INDEX);
	VoC_loop_i(1,80)
		VoC_lw16inc_p(REG3,REG0,DEFAULT);
		VoC_mac32_rr(REG3,REG3,DEFAULT);
	VoC_endloop1
    VoC_NOP();
	VoC_shr32_ri(ACC0,10,DEFAULT);
	VoC_lw32z(REG67,IN_PARALLEL);
	VoC_bez32_r(PitchFilterR2NorZero,ACC0);
	VoC_lw32_d(ACC1,PITCH_FILTER_R2_MAX);
	VoC_jal(RDA_DIV32_S);
PitchFilterR2NorZero:
    VoC_sw32_d(REG67,PITCH_FILTER_R2_NORM);



//	r_temp3=0;
//	for (k=0;k<80;k++)
//	{
//		r_temp3+=(speakerBuf_history[k+160-r3_index]*speakerBuf_history[k+160-r3_index])>>7;
//	}
////	r_temp3=sqrt(r_temp3);
//
//	r_temp3=r_temp3>>B_Qn;
//	if (r_temp3!=0) 
//	{
//		r3_norm=r3_max/r_temp3;
//	}
//	else
//	{
//		//r3_norm=0x7fffffff;
//		r3_norm=0;
	}

	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw16i_short(FORMAT32,7,IN_PARALLEL);
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	VoC_sub16_rd(REG0,REG0,PITCH_FILTER_R3_INDEX);
	VoC_loop_i(1,80)
		VoC_lw16inc_p(REG3,REG0,DEFAULT);
		VoC_mac32_rr(REG3,REG3,DEFAULT);
	VoC_endloop1
    VoC_NOP();
	VoC_shr32_ri(ACC0,10,DEFAULT);
	VoC_lw32z(REG67,IN_PARALLEL);
	VoC_bez32_r(PitchFilterR3NorZero,ACC0);
	VoC_lw32_d(ACC1,PITCH_FILTER_R3_MAX);
	VoC_jal(RDA_DIV32_S);
PitchFilterR3NorZero:
    VoC_sw32_d(REG67,PITCH_FILTER_R3_NORM);


		
//	r_M=r1_index;
//	r_norm=r1_norm;
//	
//	if (r2_norm>r_norm)
//	{
//		r_M=r2_index;
//		r_norm=r2_norm;
//	}
//	
//	if (r3_norm>r_norm)
//	{
//		r_M=r3_index;
//		r_norm=r3_norm;
// 	}

    VoC_lw32_d(ACC0,PITCH_FILTER_R1_NORM);
    VoC_lw16_d(REG0,PITCH_FILTER_R1_INDEX);
	VoC_bnlt32_rd(PitchFilterR1NorMax,ACC0,PITCH_FILTER_R2_NORM);
	VoC_lw32_d(ACC0,PITCH_FILTER_R2_NORM);
    VoC_lw16_d(REG0,PITCH_FILTER_R2_INDEX);
PitchFilterR1NorMax:
    VoC_bnlt32_rd(PitchFilterR2NorMax,ACC0,PITCH_FILTER_R3_NORM);
    VoC_lw16_d(REG0,PITCH_FILTER_R3_INDEX);
	VoC_NOP();
PitchFilterR2NorMax:
    VoC_sw16_d(REG0,PITCH_FILTER_R_M);
    

//	r_temp=0;
//	for (k=0;k<40;k++)
//	{
//		r_temp+=(speakerBuf_history[k+160]*speakerBuf_history[k+160-r1_index])>>7;
//	}
//	r_temp1=0;
//	for (k=0;k<40;k++)
//	{
//	r_temp1+=(speakerBuf_history[k+160-r_M]*speakerBuf_history[k+160-r_M])>>7;
//	}
//	printf("r_temp: %25d,r_temp1: %25d\n",r_temp,r_temp1);
//
//	r_temp1=r_temp1>>B_Qn;
//
//	if (r_temp1!=0) 
//	{
//		r_B=r_temp/r_temp1;
//	}
//	else
//	{
//		r_B=0;
// 	}
	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw32z(ACC1,IN_PARALLEL);
	VoC_lw16i_short(FORMAT32,7,DEFAULT);
	VoC_lw16i_short(FORMATX,7,IN_PARALLEL);
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_HISTORY_ADDR+160,1);
	VoC_sub16_rd(REG0,REG0,PITCH_FILTER_R_M);
	VoC_loop_i(1,40)
		VoC_lw16inc_p(REG3,REG0,DEFAULT);
		VoC_mac32_rr(REG3,REG3,DEFAULT);
		VoC_mac32inc_rp(REG3,REG1,IN_PARALLEL);
	VoC_endloop1

	VoC_NOP();
	VoC_shr32_ri(ACC0,10,DEFAULT);
	VoC_lw16i_short(REG6,0,IN_PARALLEL);
	VoC_bez32_r(PitchFilterR_B_End,ACC0);
	VoC_bngtz32_r(PitchFilterR_B_End,ACC1);
	VoC_jal(RDA_DIV32_S);
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_shru32_ri(ACC0,-10,DEFAULT);
	VoC_bngt32_rr(PitchFilterR_B_End,REG67,ACC0);
		VoC_movreg32(REG67,ACC0,DEFAULT);
        VoC_NOP();
PitchFilterR_B_End:

	VoC_sw16_d(REG6,PITCH_FILTER_R_B);

	//memcpy(micBuf_history,micBuf_history+40,40*2*4);
	
	VoC_lw16i_set_inc(REG0,PITCH_FILTER_MIC_HISTORY_ADDR+40,1);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_MIC_HISTORY_ADDR,1);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_loop_i(0,160)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0

	//memcpy(micBuf_history+40*4,micBuf+40*i,40*2);

	VoC_lw16i(REG3,40);
	VoC_mult16_rd(REG3,REG3,PITCH_FILTER_COUNT);
	VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,1);
	VoC_add16_rr(REG0,REG0,REG3,DEFAULT);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_MIC_HISTORY_ADDR+160,1);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_loop_i(0,40)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0
//	for (k=0;k<40;k++)
//	{
//		//micBuf[40*i+k]=micBuf[40*i+k]-(((r_B_old*(40-k))/40*micBuf_history[k+160-r_M_old]+(r_B*k)/40*micBuf_history[k+160-r_M])>>B_Qn);
//	micBuf[40*i+k]=micBuf[40*i+k]-((r_B_old*micBuf_history[k+160-r_M_old]-((r_B_old*k*205)>>13)*micBuf_history[k+160-r_M_old]+((r_B*k*205)>>13)*micBuf_history[k+160-r_M])>>B_Qn);
//	//	micBuf[40*i+k]=(r_B*micBuf_history[k+160-r_M])>>B_Qn;
//	
//	//	micBuf[40*i+k]=((r_B_old*(40-k))/40*micBuf_history[k+160-r_M_old]+(r_B*k)/40*micBuf_history[k+160-r_M])>>B_Qn;
//	}
//	
//	r_B_old=r_B;
//	r_M_old=r_M;

	VoC_lw16i(REG6,40);
	VoC_lw16i(REG4,205);
	
	VoC_lw16i_short(FORMAT16,13-16,DEFAULT);
	VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
	
	VoC_mult16_rd(REG6,REG6,PITCH_FILTER_COUNT);
	VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,1);
	VoC_lw16i_set_inc(REG1,PITCH_FILTER_MIC_HISTORY_ADDR+160,1);
	VoC_lw16i_set_inc(REG2,PITCH_FILTER_MIC_HISTORY_ADDR+160,1);
	VoC_lw16i_set_inc(REG3,0,1);
	VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_sub16_rd(REG1,REG1,PITCH_FILTER_R_M_OLD);
    VoC_sub16_rd(REG2,REG2,PITCH_FILTER_R_M);

	VoC_loop_i(0,40)
		VoC_lw32z(ACC0,DEFAULT);
		VoC_lw16_d(REG5,PITCH_FILTER_R_B_OLD);
		VoC_mac32_rp(REG5,REG1,DEFAULT);               //r_B_old*micBuf_history[k+160-r_M_old]
	    VoC_mult16_rr(REG7,REG4,REG3,DEFAULT);     //k*205;
		VoC_NOP();
	    VoC_multf16_rd(REG5,REG7,PITCH_FILTER_R_B_OLD);//r_B_old*k*205
		VoC_multf16_rd(REG6,REG7,PITCH_FILTER_R_B);    //(r_B*k*205)>>13
        VoC_msu32inc_rp(REG5,REG1,DEFAULT);            //-((r_B_old*k*205)>>13)*micBuf_history[k+160-r_M_old]
		VoC_NOP();
		VoC_mac32inc_rp(REG6,REG2,DEFAULT);            //+((r_B*k*205)>>13)*micBuf_history[k+160-r_M]
		VoC_NOP();
		VoC_shr32_ri(ACC0,10,DEFAULT);                 //val>>10;
		VoC_movreg16(REG5,ACC0_LO,DEFAULT);            //
		VoC_sub16_pr(REG5,REG0,REG5,DEFAULT)
		VoC_NOP();
		VoC_sw16inc_p(REG5,REG0,DEFAULT);
		VoC_inc_p(REG3,IN_PARALLEL);
	VoC_endloop0

    VoC_lw16_d(REG5,PITCH_FILTER_R_B);
    VoC_lw16_d(REG6,PITCH_FILTER_R_M);
    VoC_sw16_d(REG5,PITCH_FILTER_R_B_OLD);
    VoC_sw16_d(REG6,PITCH_FILTER_R_M_OLD);

	VoC_lw16i_short(REG5,1,DEFAULT);
	VoC_lw16i_short(REG6,3,IN_PARALLEL);
	VoC_add16_rd(REG5,REG5,PITCH_FILTER_COUNT);
    VoC_bngt16_rr(PitchFilterBegain,REG5,REG6);

	
    VoC_lw16i_set_inc(REG0,PITCH_FILTER_SPEAKER_ADDR+160,1);
    VoC_lw16i_set_inc(REG1,PITCH_FILTER_SPEAKER_ADDR,1);
	VoC_lw16inc_p(REG2,REG0,DEFAULT);
	VoC_loop_i(0,154)
		VoC_lw16inc_p(REG2,REG0,DEFAULT);
		VoC_sw16inc_p(REG2,REG1,DEFAULT);
	VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
	VoC_return;
}
*/

/**************************************************************************************
 * Function:    RDA_DIV32_S
 *
 * Description: 
 *
 * Inputs:  : 
 *			:
 *			:
 *
 * Outputs:
 *	 
 *    
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       1/17/2014
 **************************************************************************************/
/*
void RDA_DIV32_S(void)
{
    VoC_push16(RA, DEFAULT);    
    VoC_lw32z(REG67, DEFAULT);    
    VoC_lw16i_set_inc(REG1,-31,1);    
    VoC_lw16i_set_inc(REG0,31,-1);



LABLE_DIV32_START:
    VoC_movreg32(RL6, ACC1,DEFAULT); //RL6=ACC1    
    VoC_movreg32(RL7, ACC0,IN_PARALLEL); //RL7=ACC0    
    //VoC_sub16_rd(REG4,REG4, SBC_DECODE_CONST_1);//R4--
    //VoC_add16_rd(REG5,REG5, SBC_DECODE_CONST_1);//R5++
    VoC_inc_p(REG0,DEFAULT);//R4--
    VoC_inc_p(REG1,IN_PARALLEL);//R5++
    
    VoC_shru32_rr(RL6, REG0, DEFAULT); //RL6>>R6
    VoC_bgt32_rr(LABLE_DIV32_1, RL6, ACC0);
    VoC_be32_rr(LABLE_DIV32_1, RL6, ACC0);
    VoC_bnez16_r(LABLE_DIV32_START,REG0);
    VoC_bez16_r(LABLE_DIV32_END, REG0);
LABLE_DIV32_1:
    VoC_shru32_rr(RL7, REG1, DEFAULT); //RL7<<R7
    VoC_lw16i_short(RL6_HI,1,IN_PARALLEL)//RL6=1 
	VoC_shru32_ri(RL6,16,DEFAULT)
    //VoC_lw32_d(RL6, SBC_DECODE_CONST_32BIT_1);//RL6=1    
    VoC_sub32_rr(ACC1, ACC1, RL7, DEFAULT);//ACC1 = ACC1-(RL7<<R7)
    VoC_shru32_rr(RL6, REG1,IN_PARALLEL);
    VoC_add32_rr(REG67,REG67, RL6,DEFAULT);//R45+=1<<R7
    VoC_bnez16_r(LABLE_DIV32_START,REG0);

LABLE_DIV32_END:
//    VoC_movreg32(REG67, REG45, DEFAULT);
  //  VoC_NOP();       
    VoC_pop16(RA,DEFAULT);      
    VoC_NOP();
    VoC_return;

}
*/

/**************************************************************************************
 * Function:    Coolsand_HighPass
 *
 * Description: no
 *
 * Inputs:       
 *
 * Outputs: 
 *	 
 *    
 * Used : no.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       03/16/2011
 **************************************************************************************/
/*
void Coolsand_HighPassFilter(void)
{

	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);

	VoC_lw16i(REG0,28243);
	VoC_lw16i(REG1,-12411);

	VoC_lw16i(REG2,-14259);


	VoC_lw16_d(REG4,GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR);
	VoC_lw16_d(REG5,GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR);

	VoC_lw16_d(REG6,GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR);
	VoC_lw16_d(REG7,GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR);

	VoC_lw16i_short(FORMAT32,-1,DEFAULT);
	VoC_lw16i_short(FORMATX,0,DEFAULT);
	VoC_lw16i_short(FORMAT16,(6-16),DEFAULT);

	VoC_loop_i(0,160)

		VoC_multf32_rr(ACC0,REG2,REG4,DEFAULT);//-(((int)14259*(xn_1))<<1)
		VoC_lw32z(ACC1,IN_PARALLEL);

		VoC_lw16i(REG2,14259);			

		VoC_macX_rp(REG2,REG3,DEFAULT);//(int)14259*x
		VoC_macX_rr(REG0,REG6,IN_PARALLEL);//(int)28243*yn_1

		VoC_macX_rr(REG2,REG5,DEFAULT);//(int)14259*xn_2
		VoC_macX_rr(REG1,REG7,IN_PARALLEL);//-(int)12411*yn_2

		VoC_lw16i(REG2,0x0040);	

		VoC_shr32_ri(ACC0,14,DEFAULT);
		VoC_shr32_ri(ACC1,14,DEFAULT);

		VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
		VoC_movreg16(REG5,REG4,IN_PARALLEL);// xn_2=xn_1;

		VoC_shr32_ri(ACC0,-16,DEFAULT);
		VoC_lw16_p(REG4,REG3,IN_PARALLEL)// xn_1=x;

		VoC_movreg16(REG7,REG6,DEFAULT);//  yn_2=yn_1;
		VoC_movreg16(REG6,ACC0_HI,IN_PARALLEL);//  yn_1=y;

		VoC_multf16_rr(REG2,REG6,REG2,DEFAULT);

		VoC_NOP();
		
		VoC_lw16i(REG2,-14259);

		VoC_sw16inc_p(REG2,REG3,DEFAULT);			

	VoC_endloop0

	VoC_sw16_d(REG4,GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR);
	VoC_sw16_d(REG5,GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR);

	VoC_sw16_d(REG6,GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR);
	VoC_sw16_d(REG7,GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR);

	VoC_return;
}
*/

// num 0x3d7d, 0x8507, 0x3d7d, den 0x4000, 0x7ac9,0xc4d5 Q14
void Coolsand_HighPassFilter(void)
{
	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);

	VoC_lw16_d(REG0,GLOBAL_HIGH_PASS_FILTER3_DEN1_ADDR);
	VoC_lw16_d(REG1,GLOBAL_HIGH_PASS_FILTER3_DEN2_ADDR);

	VoC_lw16_d(REG2,GLOBAL_HIGH_PASS_FILTER3_NUM1_ADDR); // num[1]

	VoC_lw32_d(REG45,GLOBAL_HIGH_PASS_FILTER3_XN_1_ADDR);
	VoC_lw32_d(REG67,GLOBAL_HIGH_PASS_FILTER3_XN_2_ADDR);

	VoC_lw32_d(RL6, GLOBAL_HIGH_PASS_FILTER3_YN_1_ADDR);
	VoC_lw32_d(RL7, GLOBAL_HIGH_PASS_FILTER3_YN_2_ADDR);

	VoC_lw16i_short(FORMAT32,14,DEFAULT);
	VoC_lw16i_short(FORMATX,0,DEFAULT);
	VoC_lw16i_short(FORMAT16,(16-14),DEFAULT);

	VoC_lw16i_short(ACC1_HI, 0, DEFAULT);
	VoC_lw16i_short(ACC1_LO, 1, IN_PARALLEL);
	VoC_shr32_ri(ACC1, (1-IIR_DATA_SHIFT), DEFAULT);

	VoC_loop_i(0,160)
		VoC_push32(REG45, DEFAULT);
		VoC_push32(REG67, DEFAULT);

		// num[1] * xn_1
		VoC_shr32_ri(REG45, -2, DEFAULT);
		VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0

		VoC_multf32_rr(ACC0, REG2, REG4, DEFAULT);
		VoC_macX_rr(REG2, REG5, DEFAULT);

		VoC_lw16_p(REG5, REG3, DEFAULT);
		VoC_lw16i_short(REG4, 0, IN_PARALLEL);

		VoC_lw16_d(REG2, GLOBAL_HIGH_PASS_FILTER3_NUM2_ADDR);

		// num[0] * x
		VoC_shr32_ri(REG45, (16-IIR_DATA_SHIFT), DEFAULT);
		VoC_push16(REG3, IN_PARALLEL);

		VoC_shr32_ri(REG45, -2, DEFAULT);
		VoC_shr32_ri(REG67, -2, IN_PARALLEL);

		VoC_shru16_ri(REG4, 2, DEFAULT);
		VoC_shru16_ri(REG6, 2, IN_PARALLEL);

		VoC_mac32_rr(REG2, REG4, DEFAULT);
		VoC_macX_rr(REG2, REG5, DEFAULT);

		// num[2] * xn_2
		VoC_mac32_rr(REG2, REG6, DEFAULT);
		VoC_macX_rr(REG2, REG7, DEFAULT);

		VoC_movreg32(REG45, RL7, DEFAULT);
		VoC_movreg32(REG67, RL6, IN_PARALLEL);

		// den[2] * yn_2
		VoC_shr32_ri(REG45, -2, DEFAULT);
		VoC_shr32_ri(REG67, -2, IN_PARALLEL);

		VoC_shru16_ri(REG4, 2, DEFAULT); // split 30:14 13:0
		VoC_shru16_ri(REG6, 2, IN_PARALLEL);
		
		VoC_mac32_rr(REG1, REG4, DEFAULT);
		VoC_macX_rr(REG1, REG5, DEFAULT);

		// den[1] * yn_1
		VoC_mac32_rr(REG0, REG6, DEFAULT);
		VoC_macX_rr(REG0, REG7, DEFAULT);

		// shift y
		VoC_movreg32(RL7, RL6, DEFAULT);
		VoC_movreg32(RL6, ACC0, DEFAULT);

		VoC_add32_rr(ACC0, ACC0, ACC1, DEFAULT);
		VoC_shr32_ri(ACC0, IIR_DATA_SHIFT, DEFAULT);

		VoC_pop32(REG67, DEFAULT);
		VoC_pop32(REG45, DEFAULT);

		VoC_lw16_p(REG3, REG3, DEFAULT);
		// shift x
		VoC_movreg32(REG67, REG45, DEFAULT);
		VoC_lw16i_short(REG2, 0, IN_PARALLEL);

		VoC_shr32_ri(REG23, (16-IIR_DATA_SHIFT), DEFAULT);
		VoC_movreg32(REG45, REG23, DEFAULT);

		VoC_pop16(REG3, DEFAULT);

		// clip output pcm
		VoC_bngt32_rd(COOLSAND_HPF_CLIP_MIN_SHORT, ACC0, HPF_CLIP_MAX_SHORT)
			VoC_lw16i(ACC0_LO, 0x7fff);
			VoC_jump(COOLSAND_HPF_CLIP_END);
COOLSAND_HPF_CLIP_MIN_SHORT:
		VoC_bnlt32_rd(COOLSAND_HPF_CLIP_END, ACC0, HPF_CLIP_MIN_SHORT)
			VoC_lw16i(ACC0_LO, 0x8000);
COOLSAND_HPF_CLIP_END:

		VoC_NOP();
		VoC_sw16inc_p(ACC0_LO, REG3, DEFAULT);

		VoC_lw16_d(REG2, GLOBAL_HIGH_PASS_FILTER3_NUM1_ADDR);
	VoC_endloop0

	VoC_sw32_d(REG45,GLOBAL_HIGH_PASS_FILTER3_XN_1_ADDR);
	VoC_sw32_d(REG67,GLOBAL_HIGH_PASS_FILTER3_XN_2_ADDR);

	VoC_sw32_d(RL6,GLOBAL_HIGH_PASS_FILTER3_YN_1_ADDR);
	VoC_sw32_d(RL7,GLOBAL_HIGH_PASS_FILTER3_YN_2_ADDR);

	VoC_return;
}
