#include "vppp_speech_common.h"


// VoC_directive: PARSER_OFF
#include <stdio.h>
extern FILE *TestFileHandle;

// VoC_directive: PARSER_ON




#if 0


// voc_enum NonLinearTransform

//	voc_alias NonLinearTransform_Spk_Beta_order				  0x0000, x
  //  voc_alias NonLinearTransform_Spk_T_HI				      0x0000, x
//	voc_alias NonLinearTransform_Spk_T_LO				      0xc000, x
//	voc_alias NonLinearTransform_Spk_Scale					  0x3333, x  
	//voc_alias NonLinearTransform_Spk_CosPhaseBitNum		      0x0006, x
// voc_enum_end




     voc_struct  VPP_SPEECH_DEC_IN_STRUCT __attribute__((export)),x
	 voc_short INPUT_BITS2_ADDR[26],x
     voc_struct_end

     voc_struct  VPP_SPEECH_ENC_OUT_STRUCT __attribute__((export)),x
     voc_short OUTPUT_BITS2_ADDR,22,x
     voc_struct_end

     voc_struct VPP_SPEECH_ENC_IN_STRUCT __attribute__((export)),x
     voc_short INPUT_SPEECH_BUFFER2_ADDR,160,x
     voc_short INPUT_SPEECH_BUFFER1_ADDR,160,x
     voc_struct_end

     voc_struct   VPP_SPEECH_DEC_OUT_STRUCT   __attribute__((export)),x
     voc_short OUTPUT_SPEECH_BUFFER2_ADDR,160,x
     voc_short OUTPUT_SPEECH_BUFFER1_ADDR,160,x
	 voc_struct_end

#endif

#if 0
	voc_short GLOBAL_SDF_SAMPLS_ADDR,64,x







#endif

	#if 0

     voc_struct VPP_SPEECH_INIT_STRUCT          __attribute__((export)) , y
     voc_short GLOBAL_LOADED_VOCODER_ADDR			   ,y
     voc_short GLOBAL_LOADED_AMR_EFR_ADDR,				 y
     voc_short GLOBAL_LOADED_AMR_ENC_MODE_ADDR,          y
     voc_short GLOBAL_LOADED_AMR_DEC_MODE_ADDR,          y
     voc_short GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR,6,   y
     voc_short GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR,4,		  y
     voc_short GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR,16 ,y
     voc_short GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR,16, y
     voc_short GLOBAL_CONSTANTS_ADDR_ADDR,8, y

     voc_short GLOBAL_RESET_ADDR							,y
     voc_short GLOBAL_IRQGEN_ADDR							,y
     voc_struct_end



     voc_struct VPP_SPEECH_AUDIO_CFG_STRUCT  __attribute__((export)),y


     voc_short GLOBAL_ECHO_ES_ON,y
     voc_short GLOBAL_ECHO_ES_VAD,y
	 voc_short GLOBAL_ECHO_ES_DTD,y

     voc_short GLOBAL_ECHO_EC_REL,y
	 voc_short GLOBAL_ECHO_EC_MU,y
     voc_short GLOBAL_ECHO_EC_MIN,y

	 voc_short GLOBAL_ENC_MICGAIN_ADDR,y
     voc_short GLOBAL_DEC_SPKGAIN_ADDR,y
     voc_short GLOBAL_SDF_ADDR,2,y
     voc_short GLOBAL_MDF_ADDR,2,y
     voc_short GLOBAL_IF1_flag_ADDR     , 1 ,    y
     voc_struct_end


     voc_short GLOBAL_DTX_ADDR              ,y

     voc_short GLOBAL_RELOAD_MODE_ADDR,                  y
     voc_short GLOBAL_AMR_EFR_RESET_ADDR,                  y

     voc_short GLOBAL_OLD_MICGAIN_ADDR,y
     voc_short GLOBAL_OLD_SPKGAIN_ADDR,y
     voc_short GLOBAL_SDF_COEFFS_ADDR, 64,               y

     voc_short GLOBAL_BFI_ADDR,                          y
     voc_short GLOBAL_UFI_ADDR,                          y

     voc_short GLOBAL_SID_ADDR,                          y
     voc_short GLOBAL_TAF_ADDR,                          y
     voc_short GLOBAL_DEC_MODE_ADDR,                     y
 
     voc_short GLOBAL_ENC_MODE_ADDR,                     y
     voc_short GLOBAL_ENC_USED_MODE_ADDR,                y

     voc_short GLOBAL_EFR_FLAG_ADDR,                    y
     voc_short DEC_RESET_FLAG_ADDR,                      y
     voc_short DEC_MAIN_RESET_FLAG_OLD_ADDR,             y

#endif

#if 0
	voc_short GLOBAL_MDF_SAMPLS_ADDR,64,x

#endif



#if 0

 

 voc_struct AMR_EFR_RAMX_CONSTANTS_ADDR     ,x



	voc_short  CONST_2_ADDR          ,x
	voc_short  CONST_3_ADDR          ,x
	voc_short  CONST_4_ADDR          ,x
	voc_short  CONST_6_ADDR          ,x
	voc_short  CONST_7_ADDR          ,x
	voc_short  CONST_8_ADDR          ,x
	voc_short  CONST_9_ADDR          ,x
	voc_short  CONST_10_ADDR         ,x
	voc_short  CONST_11_ADDR         ,x
	voc_short  CONST_12_ADDR         ,x
	voc_short  CONST_13_ADDR         ,x
	voc_short  CONST_15_ADDR         ,x
	voc_short  CONST_16_ADDR         ,x
	voc_short  CONST_20_ADDR         ,x
	voc_short  CONST_30_ADDR         ,x
	voc_short  CONST_32_ADDR         ,x
	voc_short  CONST_40_ADDR         ,x
	voc_short  CONST_41_ADDR         ,x
	voc_short  CONST_64_ADDR         ,x
	voc_short  CONST_80_ADDR         ,x
	voc_short  CONST_120_ADDR        ,x
	voc_short  CONST_143_ADDR        ,x
	voc_short  CONST_160_ADDR        ,x
	voc_short  CONST_197_ADDR        ,x
	voc_short  CONST_5325_ADDR       ,x
	voc_short  CONST_0x1fff_ADDR     ,x
	voc_short  CONST_5_ADDR          ,x
	voc_short  CONST_15565_ADDR      ,x
	voc_short  CONST_0x1555_ADDR     ,x
	voc_short  CONST_0x2000_ADDR     ,x
	voc_short  CONST_neg2_ADDR       ,x
	voc_short  CONST_0x199a_ADDR     ,x
	voc_short  CONST_0x5a82_ADDR     ,x
	voc_short  CONST_0x00ff_ADDR     ,x


	voc_short  CONST_6390_ADDR        ,x                  		    	
	voc_short  CONST_21955_ADDR       ,x                   		    	
	voc_short  CONST_3641_ADDR        ,x                  		    	
	voc_short  CONST_0xf508_ADDR      ,x                    		
	voc_short  CONST_1260_ADDR        ,x                  		    	
	voc_short  CONST_720_ADDR         ,x                 		        
	voc_short  CONST_0x3fff_ADDR      ,x                    		
	voc_short  CONST_0x7f80_ADDR      ,x                    		
	voc_short  CONST_0x7c00_ADDR      ,x                    		
	voc_short  CONST_1000_ADDR        ,x                  		        
	voc_short  CONST_32750_ADDR       ,x                   		        
	voc_short  CONST_17578_ADDR       ,x                   		        
	voc_short  CONST_8521_ADDR        ,x                  		        
	voc_short  CONST_0xa80_ADDR       ,x                   		        
	voc_short  CONST_5443_ADDR        ,x                  		        
	voc_short  CONST_1310_ADDR        ,x                  		        

	voc_short CONST_28160_ADDR        ,x  
	voc_short CONST_1843_ADDR         ,x              		      
	voc_short CONST_3427_ADDR         ,x              		       
	voc_short CONST_6242_ADDR         ,x              		       
	voc_short CONST_31821_ADDR        ,x 
	voc_short CONST_3276_ADDR         ,x 
	voc_short CONST_368_ADDR		  ,x	
	voc_short CONST_21298_ADDR        ,x
	voc_short CONST_24660_ADDR        ,x
	voc_short CONST_5462_ADDR         ,x
	voc_short CONST_13106_ADDR        ,x
	voc_short CONST_1500_ADDR        ,x
 // 	voc_short  ReservedX16_ADDR     ,4,x

	voc_short  CONST_0_ADDR              ,2,x
	voc_short  CONST_1_ADDR              ,2,x
	voc_short  CONST_0x40000000_ADDR     ,2,x
	voc_short  CONST_0x7FFFFFFF_ADDR     ,2,x
	voc_short  CONST_0x80000000_ADDR     ,2,x
	voc_short  CONST_0x8000FFFF_ADDR     ,2,x
	voc_short  CONST_D_1_4_ADDR          ,2,x
	voc_short  CONST_0x00010001_ADDR     ,2,x
	voc_short  CONST_0x00004000L_ADDR    ,2,x
	voc_short  CONST_0x00007FFF_ADDR     ,2,x
	voc_short  CONST_0x00008000_ADDR     ,2,x
	voc_short  CONST_0x0000FFFF_ADDR     ,2,x


	voc_short CONST_0x70816958_ADDR    ,2,x                          
	voc_short CONST_0x69cb0014_ADDR    ,2,x                          
	voc_short CONST_15000_ADDR         ,2,x                          
	voc_short CONST_0x53c00_ADDR       ,2,x                          
	voc_short CONST_0x00040000L_ADDR   ,2,x               		
                                         

	voc_short CONST_D_0xc800_ADDR     ,2,x              		
	voc_short CONST_D_0xf6b3_ADDR     ,2,x             			
	voc_short CONST_D_150_ADDR        ,2,x            		    
	voc_short CONST_13849L_ADDR      ,2,x 
	voc_short CONST_0x10000000_ADDR    ,2,x 


	voc_short CONST_0x40000028_ADDR    ,2,x                       
	voc_short CONST_0x55550001_ADDR    ,2,x                        
	voc_short CONST_0x7fff0007_ADDR    ,2,x                        
    voc_short CONST_D_3500_ADDR       ,2,x 

	voc_short CONST_0x04000000_ADDR  ,2,x 
	voc_short CONST_0x80017fff_ADDR  ,2,x 
	voc_short  CONST_0x2000e000_ADDR ,2,x 
	voc_short	CONST_783741L_ADDR   ,2,x 
	voc_short CONST_0x100000_ADDR   ,2,x

	voc_short CONST_72_40_ADDR		,2,x                            
	voc_short CONST_40_36_ADDR		,2,x
                           
	voc_short CONST_D_1_8_ADDR      ,2,x                             
	voc_short CONST_D_1_16_ADDR     ,2,x                   	
	voc_short CONST_D_1_32_ADDR     ,2,x                  	
	voc_short CONST_D_1_64_ADDR     ,2,x  

	voc_short CONST_1_8_0x4000_ADDR  ,2,x                        
	voc_short CONST_1_16_1_4_ADDR    ,2,x                   		
	voc_short CONST_1_32_1_4_ADDR    ,2,x                   		
	voc_short CONST_1_64_1_8_ADDR    ,2,x                   		

    voc_short  CONST_0x00000100_ADDR ,2,x 
	voc_short  CONST_40_64_ADDR      ,2,x 

	voc_short CONST_0x7fff1fff_ADDR     ,2,x                          
	voc_short CONST_0x8000e000_ADDR     ,2,x                          

	voc_short CONST_ALPHA_DOWN2_ALPHA_UP2_ADDR   ,2,x  

	voc_short CONST_600_32000_ADDR     ,2,x  
	voc_short CONST_800_30500_ADDR     ,2,x 
	voc_short CONST_0xc3da1e13_ADDR   ,2,x



/*
    voc_short TABLE_DTX_psrCNNewFactor_ADDR        ,   12   ,x
	voc_short TABLE_DTX_ppLr_gsTable_ADDR          ,   256  ,x
	voc_short TABLE_ERR_psrR0RepeatThreshold_ADDR  ,   32   ,x
	voc_short TABLE_ERR_psrConceal_ADDR            ,   16   ,x
	voc_short TABLE_DE_A_SST_psrSST_ADDR           ,   12   ,x
	voc_short TABLE_ERR_psrLevelMaxIncrease_ADDR   ,   16   ,x
	voc_short TABLE_HOMING_n_ADDR                  ,   18   ,x
	voc_short TABLE_HOMING_dhf_mask_ADDR           ,   18   ,x
	voc_short TABLE_DE_SP_psrSPFDenomWidenCf_ADDR  ,   10   ,x
	voc_short TABLE_SP_FRM_pswPCoefE_ADDR          ,   4    ,x
	voc_short TABLE_SP_FRM_psrSubMultFactor_ADDR   ,   6    ,x
	voc_short TABLE_SP_ROM_pppsrGsp0_ADDR          ,   640  ,x
	voc_short TABLE_SP_ROM_pppsrUvCodeVec_ADDR     ,   560  ,x
	voc_short TABLE_SP_ROM_pppsrVcdCodeVec_ADDR    ,   360  ,x
	voc_short TABLE_SP_ROM_psrR0DecTbl_ADDR           	,64    ,x
	voc_short TABLE_SP_ROM_psrHPFCoefs_ADDR           	,10    ,x
	voc_short TABLE_SP_ROM_psrNWCoefs_ADDR            	,20    ,x
	voc_short TABLE_SP_ROM_pL_rFlatSstCoefs_ADDR      	,20    ,x
	voc_short TABLE_SP_ROM_psrOldCont_ADDR            	,4     ,x
	voc_short TABLE_SP_ROM_psrNewCont_ADDR            	,4     ,x
	voc_short TABLE_SFRM_pswUpdateIndexV_pswBitIndexV_ADDR	,256   ,x
	voc_short TABLE_SFRM_pswUpdateIndexUn_pswBitIndexUn_ADDR,64    ,x
	voc_short TABLE_SFRM_pswModNextBitV_ADDR          	,10    ,x
	voc_short TABLE_SFRM_pswModNextBitUV_ADDR         	,8     ,x
	voc_short TABLE_HOMING_n1_ADDR                    	,18    ,x

*/
 //  voc_struct_end

//	voc_short ReservedX_ADDR,  (628+818+118-6),x


     voc_short TABLE_SQRT_L_ADDR                    ,50   ,x
     voc_short STATIC_CONST_PRMNO_ADDR		        ,10   ,x
     voc_short STATIC_CONST_CDOWN_ADDR	            ,8    ,x
     voc_short STATIC_CONST_PDOWN_ADDR	            ,8    ,x

     voc_short TABLE_POW2_ADDR			           ,34    ,x
     voc_short TABLE_INV_SQRT_ADDR		           ,50    ,x
     voc_short TABLE_LSP_LSF_ADDR		           ,66    ,x
     voc_short  TABLE_SLOPE_ADDR		           ,64    ,x

     voc_short STATIC_CONST_dtx_log_en_adjust_ADDR      ,10  ,x
     voc_short STATIC_CONST_lsf_hist_mean_scale_ADDR	,10  ,x
     voc_short STATIC_CONST_DGRAY_ADDR	                ,8   ,x
     voc_short STATIC_CONST_GRAY_ADDR	                ,8   ,x
     voc_short TABLE_LOG2_ADDR			                ,34  ,x
     voc_short STATIC_CONST_qua_gain_code_ADDR          ,96  ,x
     voc_short STATIC_CONST_QUA_GAIN_PITCH_ADDR         ,16  ,x



     voc_short TABLE_GRID_ADDR			               ,62 ,x
	 voc_short STATIC_CONST_PRMNOFSF_ADDR	            ,8   ,x
	 voc_short STATIC_CONST_BITNO_ADDR	             ,30      ,x

   voc_short  STATIC_CONST_bitno_MR475_ADDR	       ,9    ,x
   voc_short  STATIC_CONST_bitno_MR515_ADDR	       ,10   ,x
   voc_short  STATIC_CONST_bitno_MR59_ADDR	       ,10   ,x
   voc_short  STATIC_CONST_bitno_MR67_ADDR	       ,10   ,x
   voc_short  STATIC_CONST_bitno_MR74_ADDR	      ,10    ,x
   voc_short  STATIC_CONST_bitno_MR795_ADDR	      ,12    ,x
   voc_short  STATIC_CONST_bitno_MR102_ADDR	      ,20    ,x
   voc_short  STATIC_CONST_bitno_MRDTX_ADDR	       ,3    ,x

    voc_short STATIC_CONST_ph_imp_mid_ADDR             ,40    ,x
    voc_short STATIC_CONST_ph_imp_low_MR795_ADDR       ,40    ,x
    voc_short STATIC_trackTable_ADDR	               ,20    ,x


    voc_short STATIC_CONST_table_gain_highrates_ADDR   ,512   ,x

    voc_short STATIC_CONST_table_gain_MR475_ADDR	   ,1024  ,x

	voc_short STATIC_mr515_3_lsf_compressed_ADDR	    ,416   ,x
    voc_short TABLE_DICO1_LSF_3_compressed_ADDR	                ,624 ,x
    voc_short TABLE_DICO3_LSF_3_compressed_ADDR	                ,1664 ,x
    voc_short TABLE_DICO1_LSF_compressed_ADDR	                   ,416   ,x


	voc_short ReservedX4_ADDR,    (2+720+8-32-32-38-480-4-15*6-16-4),x



	voc_short GLOBAL_MIC_DIGITAL_OLD_GAIN_ADDR,x
	voc_short GLOBAL_RECEIVER_DIGITAL_OLD_GAIN_ADDR,x

	voc_short GLOBAL_AEC_DIGITAL_OLD_GAIN_ADDR,x
	voc_short GLOBAL_DIGITAL_OLD_GAIN_ADDR,x


	voc_short COS_TABLE_ADDR[129],x	
	voc_short TAN_TABLE_ADDR[33+(FRAMESIZE*2-129-33)],x

	voc_alias GLOBAL_NON_CLIP_TEMP_BUFFER COS_TABLE_ADDR,x
	
	voc_short GLOBAL_NON_CLIP_MIC_HISTORYDATA[FRAMESIZE],x

	voc_short GLOBAL_NON_CLIP_CROSSZEROCOUNTER,x
	voc_short GLOBAL_NON_CLIP_CROSSZEPEAK,x

	voc_short GLOBAL_DIGITAL_GAIN_ADDR,x
	voc_short GLOBAL_DIGITAL_MAXVALUE_ADDR,x

	voc_short GLOBAL_NOTCH_FILTER_XN12_YN12_ADDR[15*6],x

	voc_short GLOBAL_NOTCH_FILTER_COEF_ADDR __attribute__((export)),x
	voc_short GLOBAL_NOTCH_FILTER_COEF_ADDR0[15],x
	

    voc_struct_end




 #endif






#if 0
     voc_short GLOBAL_MDF_COEFFS_ADDR,64,         y

// ECHO
     voc_short GLOBAL_ECHO_SUPPRESS_EXP_COUNTER,1,      y
     voc_short GLOBAL_ECHO_CANCEL_EXP_MU,1,      y

     voc_short GLOBAL_ECHO_CANCEL_DEC_PWR,2,      y


//	voc_short GLOBAL_HIGH_PASS_FILTER_XN_1_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER_XN_2_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER_YN_1_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER_YN_2_ADDR,y

	voc_short GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR,y
	voc_short GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR,y
	voc_short GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR,y
	voc_short GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR,y
	
	
		// for ns cat tools
	// now can NOISE_SUPPRESS_DEFAULT(SpxPpState_noise_suppress)
	// and SPEEX_NOISE_PROB_MIN_RANGE*(min_range) in update_noise_prob
	voc_short SPEEX_NOISE_SUPPRESS_DEFAULT_TX	__attribute__((export)),y
	voc_short SPEEX_NOISE_SUPPRESS_DEFAULT_RX	__attribute__((export)),y

	voc_short SPEEX_NOISE_PROB_MIN_TX_RANGE1	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_TX_RANGE2	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_TX_RANGE3	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_TX_RANGE4	__attribute__((export)),y
	
	voc_short SPEEX_NOISE_PROB_MIN_RX_RANGE1	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_RX_RANGE2	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_RX_RANGE3	__attribute__((export)),y
	voc_short SPEEX_NOISE_PROB_MIN_RX_RANGE4	__attribute__((export)),y

	voc_short SPEEX_NOISE_SUPPRESS_DEFAULT,y
	voc_short GLOBAL_MSDF_RELOAD_FLAG	__attribute__((export)),y

	voc_short SPEEX_NOISE_PROB_MIN_RANGE1,y
	voc_short SPEEX_NOISE_PROB_MIN_RANGE2,y
	voc_short SPEEX_NOISE_PROB_MIN_RANGE3,y
	voc_short SPEEX_NOISE_PROB_MIN_RANGE4,y

	voc_short MIC_NONCLIP_ENABLE_FLAG_ADDR	__attribute__((export)),y
	voc_short RECEIVER_NONCLIP_ENABLE_FLAG_ADDR	__attribute__((export)),y

	voc_short GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_TX	__attribute__((export)),y
	voc_short GLOBAL_NON_CLIP_NOISE_THRESHOLD_TX	__attribute__((export)),y

	voc_short GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_RX	__attribute__((export)),y
	voc_short GLOBAL_NON_CLIP_NOISE_THRESHOLD_RX	__attribute__((export)),y

	voc_short GLOBAL_NON_CLIP_SMOOTH_THRESHOLD,y
	voc_short GLOBAL_NON_CLIP_NOISE_THRESHOLD,y

	voc_word GLOBAL_TX_BEFORE_ENC_ADDR __attribute__((export)),y

	voc_short GLOBAL_DEC_OUTPUT_DELAY_BUFFER1_ADDR,50,y
	voc_short GLOBAL_DEC_OUTPUT_DELAY_BUFFER2_ADDR,50,y

	voc_short NONCLIP_ENABLE_FLAG_ADDR	__attribute__((export)),y
	voc_short NONCLIP_ENABLE_FLAG_RESERVED,y

	voc_short SCALE_MIC_ENABLE_FLAG_ADDR	__attribute__((export)),y
	voc_short SCALE_RECEIVER_ENABLE_FLAG_ADDR	__attribute__((export)),y

	voc_short GLOBAL_MIC_SCALE_IN_ADDR		__attribute__((export)),y
	voc_short GLOBAL_RECEIVER_SCALE_OUT_ADDR	__attribute__((export)),y
	
	voc_word GLOBAL_HIGH_PASS_FILTER3_XN_1_ADDR,y
	voc_word GLOBAL_HIGH_PASS_FILTER3_XN_2_ADDR,y
	voc_word GLOBAL_HIGH_PASS_FILTER3_YN_1_ADDR,y
	voc_word GLOBAL_HIGH_PASS_FILTER3_YN_2_ADDR,y

	voc_short GLOBAL_HIGH_PASS_FILTER3_NUM1_ADDR	__attribute__((export)),y
	voc_short GLOBAL_HIGH_PASS_FILTER3_NUM2_ADDR	__attribute__((export)),y
	voc_short GLOBAL_HIGH_PASS_FILTER3_DEN1_ADDR	__attribute__((export)),y
	voc_short GLOBAL_HIGH_PASS_FILTER3_DEN2_ADDR	__attribute__((export)),y

	voc_word HPF_CLIP_MAX_SHORT,y
	voc_word HPF_CLIP_MIN_SHORT,y

	voc_short GLOBAL_COMFORTALBE_NOISE_ADDR,160-16-10-50-50-2-2-2-12-4,y

	voc_short GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR,y
	voc_short GLOBAL_VAD_COUNTER,y

	voc_short GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR,y
	voc_short GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR,y

	voc_short GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR,y
	voc_short GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR,y


//	voc_short GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR,y
//	voc_short GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR,y

	voc_short GLOBAL_MIC_DIGITAL_GAIN_ADDR __attribute__((export)),y
	voc_short GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR __attribute__((export)),y
	voc_short GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR __attribute__((export)),y
	voc_short GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR __attribute__((export)),y
		
	voc_word GLOBAL_OUTPUTENERGY_ADDR __attribute__((export)),y
	voc_word GLOBAL_INPUTENERGY_ADDR __attribute__((export)),y

voc_struct m_IIRProcPara_ADDR __attribute__((export)),y
	voc_short m_IIRProcPara_band0_coeffs[6]

	voc_short m_IIRProcPara_band1_coeffs[6]

	voc_short m_IIRProcPara_band2_coeffs[6]

	voc_short m_IIRProcPara_band3_coeffs[6]

	voc_short m_IIRProcPara_band4_coeffs[6]

	voc_short m_IIRProcPara_band5_coeffs[6]

	voc_short m_IIRProcPara_band6_coeffs[6]
voc_struct_end

	// Rda_ProcessIIRBand
	voc_short IIR_ENABLE_FLAG_ADDR __attribute__((export)),y
	voc_short IIR_INPUT_ADDR_ADDR,y

	voc_struct m_AECExtraPara_ADDR __attribute__((export)),y
		voc_short m_AECProcPara_DelaySampleNum

		voc_short m_AECProcPara_FrameCntLimitNum[3]
		voc_short m_AECProcPara_NoiseCntLimitNum[4]
		//voc_short m_AECProcPara_NoiseGainLimitMax
		//voc_short m_AECProcPara_NoiseGainLimitMin
		//voc_short m_AECProcPara_NoiseGainLimitSNRTh1
		//voc_short m_AECProcPara_NoiseGainLimitSNRTh2

        voc_short m_AECProcPara_NoiseWeightSNRLimitMax
        voc_short m_AECProcPara_NoiseWeightSNRLimitMin
        voc_short m_AECProcPara_NoiseWeightSNRTh1
        voc_short m_AECProcPara_NoiseWeightSNRTh2
        voc_short m_AECProcPara_NoiseWeightSNRLimitStep
        voc_short m_AECProcPara_NoiseWeightSub1DecayFactor

		voc_short m_AECProcPara_ResidualEchoEnable
		voc_short m_AECProcPara_AGCRrrSpkRelation
		voc_short m_AECProcPara_AddAttDB
		voc_short m_AECProcPara_MinAttDB
		voc_short m_AECProcPara_MaxAttDB
		voc_short m_AECProcPara_NPowRiseFactor
		voc_short m_AECProcPara_WorkFactFactor
		voc_short m_AECProcPara_AttExtFactor
		voc_short m_AECProcPara_AttRiseFactor
		voc_short m_AECProcPara_AttDecayFactor
		voc_short m_AECProcPara_Log2LinFactor
		voc_short m_AECProcPara_Reversed
	voc_struct_end

	//voc_short GLOBAL_AECExtraPara_Reversed,46,y
    voc_short GLOBAL_AECExtraPara_Reversed,44,y

	voc_word GLOBAL_AGC_CODE_ADDR __attribute__((export)),y
	voc_word GLOBAL_AGC_CONSTX_ADDR __attribute__((export)),y
	voc_alias AGC1_ConstY_ExternalAddr_addr GLOBAL_AGC_CONSTX_ADDR
	voc_word GLOBAL_MIC_AGC_RAMX_BACKUP_ADDR __attribute__((export)),y
	voc_word GLOBAL_RECEIVER_AGC_RAMX_BACKUP_ADDR __attribute__((export)),y
	
	voc_short GLOBAL_MICAGC_RELOAD __attribute__((export)),y
	voc_short GLOBAL_RECEIVERAGC_RELOAD __attribute__((export)),y

	voc_struct STRUCT_MicAgcConfig_addr __attribute__((export)),y
		voc_short STRUCT_MicAgcConfig_enable_addr
		voc_short STRUCT_MicAgcConfig_targetLevelDbfs_addr      // default 3 (-3 dBOv)
		voc_short STRUCT_MicAgcConfig_compressionGaindB_addr    // default 9 dB
		voc_short STRUCT_MicAgcConfig_limiterEnable_addr        // default kAgcTrue (on)
		voc_short STRUCT_MicAgcConfig_upperThr_addr
		voc_short STRUCT_MicAgcConfig_lowerThr_addr
		voc_short STRUCT_MicAgcConfig_decayValue_addr
		voc_short STRUCT_MicAgcConfig_capacitorSlowFactor_addr
		voc_short STRUCT_MicAgcConfig_capacitorFastFactor_addr
		voc_short STRUCT_MicAgcConfig_NoiseCompressFactor_addr
		voc_short STRUCT_MicAgcConfig_NoiseGateThr_addr
		voc_short STRUCT_MicAgcConfig_NoiseStdShortTermFactor_addr
	voc_struct_end

	voc_struct STRUCT_ReceiverAgcConfig_addr __attribute__((export)),y
		voc_short STRUCT_ReceiverAgcConfig_enable_addr
		voc_short STRUCT_ReceiverAgcConfig_targetLevelDbfs_addr      // default 3 (-3 dBOv)
		voc_short STRUCT_ReceiverAgcConfig_compressionGaindB_addr    // default 9 dB
		voc_short STRUCT_ReceiverAgcConfig_limiterEnable_addr        // default kAgcTrue (on)
		voc_short STRUCT_ReceiverAgcConfig_upperThr_addr
		voc_short STRUCT_ReceiverAgcConfig_lowerThr_addr
		voc_short STRUCT_ReceiverAgcConfig_decayValue_addr
		voc_short STRUCT_ReceiverAgcConfig_capacitorSlowFactor_addr
		voc_short STRUCT_ReceiverAgcConfig_capacitorFastFactor_addr
		voc_short STRUCT_ReceiverAgcConfig_NoiseCompressFactor_addr
		voc_short STRUCT_ReceiverAgcConfig_NoiseGateThr_addr
		voc_short STRUCT_ReceiverAgcConfig_NoiseStdShortTermFactor_addr
	voc_struct_end

	voc_short GLOBAL_MICFIR_ENABLE_FLAG_ADDR __attribute__((export)),y
	voc_short GLOBAL_RECEIVERFIR_ENABLE_FLAG_ADDR __attribute__((export)),y

	voc_short GLOBAL_RESERVE_Y_ADDR,160-2-42-2-10-4-56-8-12-2-8-2-4,y

	voc_short GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR __attribute__((export)),y
	voc_short GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR __attribute__((export)),y

	
	voc_word GLOBAL_SPEECH_RAMX_BACKUP_ADDR __attribute__((export)),y	
	voc_word GLOBAL_SPEECH_CODE_BACKUP_ADDR __attribute__((export)),y
	
	voc_word GLOBAL_AEC_RAMX_BACKUP_ADDR __attribute__((export)),y
	voc_word GLOBAL_AEC_CODE_ADDR __attribute__((export)),y

	voc_word GLOBAL_AEC_CONSTX_ADDR __attribute__((export)),y	

	voc_word GLOBAL_RX_ADDR __attribute__((export)),y
	voc_word GLOBAL_TX_ADDR __attribute__((export)),y


	voc_word GLOBAL_NOISESUPPRESS_CODE_ADDR __attribute__((export)),y
	voc_word GLOBAL_NOISESUPPRESS_CONSTX_ADDR __attribute__((export)),y

	voc_word GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR __attribute__((export)),y
	voc_word GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR __attribute__((export)),y


voc_struct m_AECProcPara_ADDR __attribute__((export)),y

	voc_short AEC_ENABLE_FLAG_ADDR 
	voc_short AGC_ENABLE_FLAG_ADDR 

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

	voc_short m_AECProcPara__PFCLDTDThrRatio;
//	voc_short m_AECProcPara_NoiseCntLimitNum[4];
//	voc_short m_AECProcPara__reserve;



voc_struct_end

	voc_short GLOBAL_SPEECH_RECORD_FLAG __attribute__((export)),y


	voc_short GLABAL_AEC_RESET_ADDR,y
	voc_short GLABAL_MORPH_RESET_ADDR,y

	voc_short MORPHVOICE_ENABLE_FLAG_ADDR __attribute__((export)),y
	voc_short PITCH_SHIFT_ADDR __attribute__((export)),y
	

	voc_word GLOBAL_MORPH_CODE_ADDR __attribute__((export)),y
	voc_word GLOBAL_MORPH_CONSTX_ADDR __attribute__((export)),y

	voc_short GLABAL_NOISESUPPRESS_RESET_ADDR,y
	voc_short GLABAL_NOISESUPPRESS_INPUTADDR_ADDR,y
		

	voc_short ingSumPhase_ADDR[129],y
	voc_short gRover_ADDR,y




//	voc_short GLOBAL_RESERVE_ADDR[381-5-160-2-10-4-8-4-18-4-4-5-1-4-6-130-2-4-6+10-4-2-4-2-1-2]				,y

    // voc_short GLOBAL_ECHO_CANCEL_SAMPLS_ADDR,296,y
    // voc_short GLOBAL_ECHO_CANCEL_COEFFS_ADDR,256,y



	 voc_short GLOBAL_VOCODER_RESET_ADDR,      y

     voc_short GLOBAL_OUTPUT_Vad_ADDR,            y
     voc_short GLOBAL_OUTPUT_SP_ADDR,             y
     voc_short DEC_AMR_FRAME_TYPE_ADDR,           y

     voc_short ENC_INPUT_ADDR_ADDR          ,y
	 voc_short DEC_OUTPUT_ADDR_ADDR         ,y
     voc_short ENC_INPUT_ADDR_BAK_ADDR      ,y
     voc_short DEC_OUTPUT_ADDR_BAK_ADDR     ,y



	#endif
/*

        #if 0

      voc_struct HR_RAMY_CONSTANTS_ADDR     ,y

	voc_short TABLE_SP_ROM_psrQuant1_ADDR             ,3072    ,    y
	voc_short TABLE_SP_ROM_psrQuant2_ADDR             ,768     ,    y
	voc_short TABLE_SP_ROM_psrQuant3_ADDR             ,512     ,    y
	voc_short TABLE_SP_ROM_psrPreQ1_ADDR              ,96      ,    y
	voc_short TABLE_SP_ROM_psrPreQ2_ADDR              ,48      ,    y
	voc_short TABLE_SP_ROM_psrPreQ3_ADDR              ,32      ,    y
	voc_short TABLE_SP_ROM_psrQuantSz_ADDR            ,4       ,    y
	voc_short TABLE_SP_ROM_psrPreQSz_ADDR             ,4       ,    y
	voc_short TABLE_SP_ROM_psrSQuant_ADDR             ,256     ,    y
	voc_short TABLE_SP_ROM_psvqIndex_ADDR             ,12      ,    y
	voc_short TABLE_SP_ROM_ppsrSqrtP0_ADDR            ,96      ,    y
	voc_short TABLE_SP_ROM_ppsrPVecIntFilt_ADDR       ,60      ,    y
	voc_short TABLE_SP_ROM_psrLagTbl_ADDR             ,256     ,    y
	voc_short TABLE_SP_ROM_ppsrCGIntFilt_ADDR         ,36      ,    y
    voc_short hr_reset_value_ADDR                    ,    10   ,   y

        voc_struct_end
        voc_short Reserved2_ADDR,           6144-5730-10+484-5,    y
    #endif
  */




      #if 0

    voc_struct AMR_EFR_RAMY_CONSTANTS_ADDR     ,y

     voc_short STATIC_CONST_F_GAMMA1_ADDR                 ,10      ,    y
     voc_short STATIC_CONST_F_GAMMA2_ADDR                 ,10      ,    y
     voc_short STATIC_CONST_F_GAMMA3_ADDR	          ,10      ,    y
     voc_short STATIC_CONST_F_GAMMA4_ADDR	          ,10      ,    y
     voc_short STATIC_CONST_gamma3_ADDR                   ,10      ,    y
     voc_short STATIC_CONST_gamma1_ADDR                   ,10      ,    y
     voc_short STATIC_CONST_WIND_200_40_compressd_ADDR              ,122     ,    y

     voc_short STATIC_CONST_INTER_6_ADDR	           ,62      ,   y
     voc_short STATIC_CONST_INTER_6_25_ADDR               ,26      ,    y

     voc_short TABLE_LAG_L_ADDR			         ,10  	   ,    y
     voc_short TABLE_LAG_H_ADDR			         ,10       ,    y
     voc_short STATIC_CONST_pred_ADDR                     ,4       ,    y
     voc_short STATIC_pred_fac_ADDR	                  ,10      ,    y
     voc_short STATIC_past_rq_init_ADDR                   ,80      ,    y
     voc_short STATIC_MEAN_LSF_3_ADDR	                  ,10      ,    y

	 voc_short STATIC_CONST_lsp_init_data_ADDR          ,10    ,y
     voc_short TABLE_MEAN_LSF_ADDR		              ,10  	      ,y


    voc_short  TABLE_WINDOW_160_80_compressd_ADDR	              ,122       ,y

    voc_short  TABLE_WINDOW_232_8_compressd_ADDR		          ,126       ,y

    voc_short STATIC_CONST_pred_MR122_ADDR	          ,4          ,y

    voc_short STATIC_CONST_INTERP_FACTOR_ADDR           ,24  	  ,y
    voc_short STATIC_CONST_SID_CODEWORD_BIT_IDX_ADDR     ,96      ,y
	voc_short STATIC_CONST_DHF_MASK_ADDR                 ,58      ,y
    voc_short STATIC_CONST_dhf_MR122_ADDR              ,58   ,y


     voc_short STATIC_CONST_dhf_MR475_ADDR              ,18  ,y
     voc_short STATIC_CONST_dhf_MR515_ADDR              ,20  ,y
     voc_short STATIC_CONST_dhf_MR59_ADDR	          ,20    ,y
     voc_short STATIC_CONST_dhf_MR67_ADDR	          ,20    ,y
     voc_short STATIC_CONST_dhf_MR74_ADDR	          ,20    ,y
     voc_short STATIC_CONST_dhf_MR795_ADDR              ,24  ,y
     voc_short STATIC_CONST_dhf_MR102_ADDR              ,40  ,y

     voc_short  STATIC_CONST_corrweight_ADDR            ,128   ,y
    voc_short  STATIC_CONST_startPos_ADDR	          ,16     ,y
 	voc_short STATIC_CONST_ph_imp_low_ADDR             ,40    ,y

    voc_short STATIC_CONST_table_gain_lowrates_ADDR	    ,256   ,y
    voc_short TABLE_DICO2_LSF_3_compressed_ADDR	        ,1248    ,y
    voc_short TABLE_DICO2_LSF_compressed_ADDR	        ,832    	  ,y
    voc_short TABLE_DICO3_LSF_compressed_ADDR	        ,832   	  ,y
    voc_short TABLE_DICO4_LSF_compressed_ADDR	        ,832    	  ,y
    voc_short TABLE_DICO5_LSF_compressed_ADDR	        ,208        ,y
	voc_short STATIC_mr795_1_lsf_compressed_ADDR	    ,1248   ,y

	voc_short TABLE_MR475_ADDR                          ,96      ,y
	voc_short TABLE_MR515_ADDR                          ,104     ,y
	voc_short TABLE_MR59_ADDR                           ,118     ,y
	voc_short TABLE_MR67_ADDR                           ,134     ,y
	voc_short TABLE_MR74_ADDR                           ,148     ,y
	voc_short TABLE_MR795_ADDR                          ,160     ,y
	voc_short TABLE_MR102_ADDR                          ,204     ,y
	voc_short TABLE_MR122_ADDR                          ,244     ,y

    voc_short Frame_header_ADDR                          ,8     ,y
    voc_short Bits_no_ADDR                              ,8     ,y

    voc_short Reserved2_ADDR,           (300-8-8-32-160-16),    y

	
	//voc_short GLOBAL_OUTPUT_BACKUP_ADDR,160,y

	voc_short GLOBAL_NON_CLIP_SPK_HISTORYDATA[FRAMESIZE],y	

	voc_short GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG __attribute__((export)),y
	voc_short GLOBAL_EXTERN_PROCESS_MIC_ENABLE_FLAG __attribute__((export)),y

	
	voc_short HIGH_PASS_FILTER_ENABLE_FLAG_ADDR __attribute__((export)),y
	voc_short NOTCH_FILTER_ENABLE_FLAG_ADDR __attribute__((export)),y

	voc_short MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR __attribute__((export)),y
	voc_short RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR __attribute__((export)),y

	//voc_word GLOBAL_AGC_acc_error_l,y

	voc_short GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR,y
	voc_short GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR,y


//	voc_word GLOBAL_AGC_MAXVAL                          ,y

	voc_word GLOBAL_MORPH_RAMX_BACKUP_ADDR __attribute__((export)),y	
	voc_word GLOBAL_AGC_Coeff_l							,y

	voc_short GLOBAL_MIC_ENERGY_COUNTER_ADDR,y
	voc_short GLOBAL_NOISE_DIGITAL_GAIN_ADDR,y

	voc_short GLOBAL_AEC_SPK_DIGITAL_GAIN_ADDR __attribute__((export)),y
	voc_short GLOBAL_AEC_SPK_DIGITAL_MAXVALUE_ADDR __attribute__((export)),y

	
	
	voc_struct_end


    #endif


#if 0

  voc_alias CONST_0x7FFF_ADDR                                 	CONST_0x00007FFF_ADDR   ,x
  voc_alias CONST_0xFFFF_ADDR                                 	CONST_0x0000FFFF_ADDR   ,x
  voc_alias CONST_0x8000_ADDR                                 	CONST_0x00008000_ADDR   ,x
  voc_alias CONST_0x4000_ADDR                                 	CONST_0x00004000L_ADDR  ,x
  voc_alias CONST_0x5999_ADDR                                 	(6+  STATIC_CONST_CDOWN_ADDR),x
  voc_alias CONST_0x4CCC_ADDR                                 	(148+  STATIC_CONST_table_gain_lowrates_ADDR),y

#endif

void CII_VOCODER_OPEN(void)
{
	// Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);
	VoC_cfg(CFG_CTRL_IRQ_TO_BCPU|CFG_CTRL_CONTINUE);
	VoC_NOP();
}

void CII_VOCODER(void)
{


CII_VOCODER_EXIT:

	// Stall the execution
	VoC_cfg(CFG_CTRL_STALL);

	// VoC_directive: PARSER_OFF
	VoC_cfg_stop;
	// VoC_directive: PARSER_ON

	VoC_NOP();
	VoC_NOP();

	// Here the execution is woken up
  	VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);

	// The wakeup mask is set on ifc0/sof0 or on ifc1/sof1 only.
	// so that we collect the 0/1 events together.
	// For this reason we get the wakeup status
	// (and no the cause = maksed status) in REG7
	VoC_lw16_d(REG7,CFG_WAKEUP_STATUS);
	VoC_lw16i_short(REG6,16,DEFAULT);

	// clear the wakeup status
	VoC_sw16_d(REG7,CFG_WAKEUP_STATUS);

	// if the events are SW, shift by 4
	VoC_bgt16_rr(is_hw_event,REG6,REG7);
		VoC_shr16_ri(REG7,4,DEFAULT);
is_hw_event:

	VoC_bez16_r(CII_VOCODER_EXIT,REG7)

	VoC_lw32_d(ACC0,GLOBAL_ENC_MICGAIN_ADDR);
	VoC_lw16i_short(REG2,-1,DEFAULT);

	VoC_lw16_d(REG3,GLOBAL_RESET_ADDR);
	//if (reset_flag != 0)
	VoC_bez16_r(CII_MAIN_VOCODER,REG3)

		VoC_lw16i_short(REG0,0,DEFAULT);
		VoC_lw16i_short(BITCACHE,0,DEFAULT);
		VoC_sw16_d(REG0,CFG_DMA_WRAP);
		VoC_lw16i(STATUS,STATUS_CLR);	
		
		// ramp mode (00 or 11)
        // mic gain mode (00 or 11)
		VoC_sw32_d(ACC0,GLOBAL_OLD_MICGAIN_ADDR);

		VoC_sw16_d(REG2,GLOBAL_RELOAD_MODE_ADDR);
		VoC_sw16_d(REG2,GLOBAL_VOCODER_RESET_ADDR);
		VoC_sw16_d(REG2,GLOBAL_AMR_EFR_RESET_ADDR);

        VoC_lw16i(REG2, INPUT_SPEECH_BUFFER2_ADDR);
        VoC_lw16i(REG3, OUTPUT_SPEECH_BUFFER2_ADDR);

        VoC_lw16i(REG4, INPUT_SPEECH_BUFFER1_ADDR);
        VoC_lw16i(REG5, OUTPUT_SPEECH_BUFFER1_ADDR);

        VoC_sw32_d(REG23,ENC_INPUT_ADDR_ADDR);
        VoC_sw32_d(REG45,ENC_INPUT_ADDR_BAK_ADDR);

        // clear GLOBAL_SDF_SAMPLS_ADDR and GLOBAL_MDF_SAMPLS_ADDR buffers
        VoC_lw16i_set_inc(REG2, GLOBAL_SDF_SAMPLS_ADDR,2);
        VoC_lw32z(ACC0,DEFAULT);
        VoC_loop_i(0,64)
            VoC_sw32inc_p(ACC0,REG2,DEFAULT);
        VoC_endloop0

        VoC_sw32_d(ACC0,CONST_0_ADDR);
        VoC_lw16i(ACC0_HI,0x4000);
		VoC_lw16i_short(REG6,15,DEFAULT);
		VoC_sw32_d(ACC0,CONST_0x40000000_ADDR);
		VoC_sw16_d(REG6,CONST_15_ADDR);

		VoC_lw32z(ACC0,DEFAULT);

	//	VoC_sw32_d(ACC0,GLOBAL_HIGH_PASS_FILTER_XN_1_ADDR);
		//VoC_sw16_d(ACC0_HI,GLOBAL_HIGH_PASS_FILTER_XN_2_ADDR);
	//	VoC_sw32_d(ACC0,GLOBAL_HIGH_PASS_FILTER_YN_1_ADDR);
		//VoC_sw16_d(ACC0_HI,GLOBAL_HIGH_PASS_FILTER_YN_2_ADDR);

	//	VoC_sw32_d(ACC0,GLOBAL_HIGH_PASS_FILTER2_XN_1_ADDR);
	//	VoC_sw16_d(ACC0_HI,GLOBAL_HIGH_PASS_FILTER2_XN_2_ADDR);
	//	VoC_sw32_d(ACC0,GLOBAL_HIGH_PASS_FILTER2_YN_1_ADDR);
	//	VoC_sw16_d(ACC0_HI,GLOBAL_HIGH_PASS_FILTER2_YN_2_ADDR);

		VoC_NOP();
		VoC_sw32_d(ACC0, GLOBAL_HIGH_PASS_FILTER3_XN_1_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_HIGH_PASS_FILTER3_XN_2_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_HIGH_PASS_FILTER3_YN_1_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_HIGH_PASS_FILTER3_YN_2_ADDR);

		VoC_lw16i(ACC0_LO, 0x7fff);
		VoC_lw16i(ACC0_HI, 0);
		VoC_lw16i(ACC1_LO, 0x8000);
		VoC_lw16i(ACC1_HI, 0xffff);
		VoC_sw32_d(ACC0, HPF_CLIP_MAX_SHORT);
		VoC_sw32_d(ACC1, HPF_CLIP_MIN_SHORT);

		/* clear iir history data
		VoC_sw32_d(ACC0, GLOBAL_IIR_BAND_XN_1_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_IIR_BAND_XN_2_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_IIR_BAND_YN_1_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_IIR_BAND_YN_2_ADDR);
		*/

		//VoC_sw16_d(ACC0_HI,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR);

		//VoC_sw16_d(ACC0_HI,GLOBAL_MIC_ENERGY_COUNTER_ADDR);
		//VoC_sw16_d(ACC0_HI,GLOBAL_VAD_COUNTER);


		//VoC_sw32_d(ACC0,GLOBAL_AGC_acc_error_l);

	//	VoC_lw16i(REG4,0xffff);
	//	VoC_lw16i_short(REG5,1,DEFAULT);
	//	VoC_lw16i(REG2,30000);
	//	VoC_lw16i_short(REG3,0,DEFAULT);

		//VoC_sw32_d(REG45,GLOBAL_AGC_Coeff_l);
		//VoC_sw32_d(REG23,GLOBAL_AGC_MAXVAL);

			
		VoC_lw16i_short(WRAP0,0,DEFAULT);
		VoC_lw16i_short(WRAP1,0,IN_PARALLEL);

		VoC_lw16i_short(WRAP2,0,DEFAULT);
		VoC_lw16i_short(WRAP3,0,IN_PARALLEL);

		//VoC_lw16i_set_inc(REG3,GLOBAL_COMFORTALBE_NOISE_ADDR,2);
		VoC_lw16i_set_inc(REG2,GLOBAL_NON_CLIP_SPK_HISTORYDATA,2);
		VoC_lw16i_set_inc(REG1,GLOBAL_NON_CLIP_MIC_HISTORYDATA,2);
		VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_BAK_ADDR,2);
		
		VoC_lw32z(ACC0,DEFAULT);

		VoC_loop_i(0,80)
            //VoC_sw32inc_p(ACC0,REG3,DEFAULT);
			VoC_sw32inc_p(ACC0,REG2,DEFAULT);
			VoC_sw32inc_p(ACC0,REG1,DEFAULT);
			VoC_sw32inc_p(ACC0,REG0,DEFAULT);
        VoC_endloop0


		VoC_sw32_d(ACC0,GLOBAL_MIC_DIGITAL_OLD_GAIN_ADDR);

		VoC_sw32_d(ACC0,GLOBAL_AEC_DIGITAL_OLD_GAIN_ADDR);
			

		VoC_lw16i_set_inc(REG2,GLOBAL_NOTCH_FILTER_XN12_YN12_ADDR,2);

		VoC_loop_i(0,45)
            VoC_sw32inc_p(ACC0,REG2,DEFAULT);
        VoC_endloop0

		VoC_lw16i_short(REG5,1,DEFAULT);
		//VoC_sw16_d(ACC0_HI,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER);
		VoC_sw16_d(REG5,GLABAL_AEC_RESET_ADDR);
		VoC_sw16_d(REG5,GLABAL_MORPH_RESET_ADDR);

		VoC_sw16_d(REG5,GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR);
		VoC_sw16_d(REG5,GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR);
		



CII_MAIN_VOCODER:

	// SAVE PARAMETERS
    VoC_movreg16(REG6,REG7,DEFAULT);
	VoC_and16_ri(REG6,1);

	VoC_bez16_r(CII_DECODER_RUN,REG6)

		//store  Mode value, Not push to RAM_X to avoid stack crash in MR74 and MR795
		VoC_push32(REG67,DEFAULT);

		VoC_lw16_d(REG2,ENC_INPUT_ADDR_ADDR);

		VoC_lw16i(REG0,INPUT_SPEECH_BUFFER2_ADDR);

		VoC_lw32_d(REG45,GLOBAL_TX_ADDR);

		VoC_be16_rr(CII_MAIN_VOCODER_L0,REG0,REG2)

			VoC_lw16i(ACC0_HI,320);
			VoC_shr32_ri(ACC0,16,DEFAULT);
			VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

	CII_MAIN_VOCODER_L0:			
			VoC_shr16_ri(REG2,1,DEFAULT);
			VoC_lw16i(REG3,80);
			VoC_jal(Rda_RunDma);

	//HPF and AEC in aec.c
	VoC_bnez16_d(Coolsand_EncoderLoadAecTab,HIGH_PASS_FILTER_ENABLE_FLAG_ADDR)
	VoC_bez16_d(Coolsand_EncoderSkipAecTab,AEC_ENABLE_FLAG_ADDR)
Coolsand_EncoderLoadAecTab:
		VoC_jal(Coolsand_Aec);
Coolsand_EncoderSkipAecTab:

	VoC_bez16_d(Coolsand_Encoder_NotchFilter,NOTCH_FILTER_ENABLE_FLAG_ADDR)
		VoC_jal(Coolsand_NotchFilter);
Coolsand_Encoder_NotchFilter:

	VoC_bez16_d(Rda_Encoder_NoiseSuppress,MIC_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)
		VoC_jal(Rda_SpeexMicNoiseSuppresser);
Rda_Encoder_NoiseSuppress:
	
	VoC_bnez16_d(Rda_Encoder_Nonclip,MIC_NONCLIP_ENABLE_FLAG_ADDR)
	VoC_bez16_d(Rda_Encoder_Agc,STRUCT_MicAgcConfig_enable_addr)
Rda_Encoder_Nonclip:
		VoC_jal(Rda_MicAgc);
Rda_Encoder_Agc:

	//Morph		
	VoC_bez16_d(Rda_Encoder_Morph,MORPHVOICE_ENABLE_FLAG_ADDR)
		VoC_jal(Coolsand_Morph);
Rda_Encoder_Morph:

	// scale mic
	VoC_bez16_d(Rda_Encoder_Scale_MIC,SCALE_MIC_ENABLE_FLAG_ADDR)
		VoC_lw16d_set_inc(REG3, ENC_INPUT_ADDR_ADDR, 1);
		VoC_lw16_d(REG0, GLOBAL_MIC_SCALE_IN_ADDR);
		VoC_lw16i_short(FORMAT32, 14, DEFAULT);
		VoC_loop_i(0, 160)
			VoC_multf32_rp(ACC0, REG0, REG3, DEFAULT);
			VoC_NOP();
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG3, DEFAULT);
		VoC_endloop0
Rda_Encoder_Scale_MIC:

	//extern mic process		
	VoC_bez16_d(Rda_Encoder_Extern_Process,GLOBAL_EXTERN_PROCESS_MIC_ENABLE_FLAG)
		VoC_jal(Rda_ProcessMic);
Rda_Encoder_Extern_Process:

// VoC_directive: PARSER_OFF
	/*
	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);
	VoC_NOP();
	VoC_NOP();
	fwrite(&RAM_X[((REGS[3].reg==INPUT_SPEECH_BUFFER1_ADDR) ? INPUT_SPEECH_BUFFER1_ADDR : INPUT_SPEECH_BUFFER2_ADDR)/2],1,320,TestFileHandle);
	fflush(TestFileHandle);
	*/
// VoC_directive: PARSER_ON

		
		VoC_lw32_d(REG67,INPUT_BITS2_ADDR); //REG6 : dtxOn,  REG7 : encModeNext

		VoC_jal(CII_MAIN_ENCODER);


//		VoC_lw16i_short(REG3,0,DEFAULT);
//				
//		VoC_bez16_d(CII_ENCODER_VAD,GLOBAL_OUTPUT_Vad_ADDR)
//
//			VoC_sw16_d(REG3,GLOBAL_VAD_COUNTER);
//CII_ENCODER_VAD:
//
//		// update noise for adding comfortable noise in loudspeaker mode
//		VoC_bnez16_d(CII_ENCODER100,GLOBAL_OUTPUT_Vad_ADDR)
//					
//			VoC_lw16i_short(REG2,1,DEFAULT);
//			
//			VoC_add16_rd(REG2,REG2,GLOBAL_VAD_COUNTER);
//
//			VoC_lw16i_short(REG3,20,DEFAULT);
//
//			VoC_sw16_d(REG2,GLOBAL_VAD_COUNTER);
//			
//			VoC_bne16_rd(CII_ENCODER_VAD_L0,REG3,GLOBAL_VAD_COUNTER);
//
//			VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);
//
//			VoC_lw32z(ACC0,DEFAULT);
//			VoC_lw16i_short(FORMAT32,6,IN_PARALLEL);
//
//			VoC_loop_i(0,80)
//				VoC_bimac32inc_pp(REG0,REG0);
//			VoC_endloop0;
//
//			VoC_lw16i_short(ACC1_LO,0x0000,DEFAULT);
//			VoC_lw16i_short(ACC1_HI,0x0020,IN_PARALLEL);
//			
//			//VoC_lw16i_short(FORMAT32,6,DEFAULT);
//
//			VoC_bgt32_rr(CII_ENCODER_VAD_L0,ACC0,ACC1)
//
//			VoC_lw16d_set_inc(REG2,ENC_INPUT_ADDR_ADDR,2);
//			VoC_lw16i_set_inc(REG3,GLOBAL_COMFORTALBE_NOISE_BAK_ADDR,2);
//			VoC_lw32inc_p(ACC0,REG2,DEFAULT);
//			VoC_loop_i(0,80)
//				VoC_lw32inc_p(ACC0,REG2,DEFAULT);
//				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
//			VoC_endloop0;
//
//CII_ENCODER_VAD_L0:
//
//			VoC_lw16i(REG3,40);
//
//			VoC_bgt16_rd(CII_ENCODER100,REG3,GLOBAL_VAD_COUNTER);
//
//			VoC_lw16i_short(REG3,0,DEFAULT);
//			VoC_lw16i_set_inc(REG2,GLOBAL_COMFORTALBE_NOISE_BAK_ADDR,2);
//			VoC_sw16_d(REG3,GLOBAL_VAD_COUNTER);
//			VoC_lw16i_set_inc(REG3,GLOBAL_COMFORTALBE_NOISE_ADDR,2);
//			VoC_lw32inc_p(ACC0,REG2,DEFAULT);
//			VoC_loop_i(0,80)
//				VoC_lw32inc_p(ACC0,REG2,DEFAULT);
//				VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
//			VoC_endloop0;
// CII_ENCODER100:


	VoC_bez16_d(CII_SPEECH_RECORD_L0,GLOBAL_SPEECH_RECORD_FLAG)	
		VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,1);
		VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,1);

		VoC_loop_i(0,160)
			VoC_lw16inc_p(REG2,REG0,DEFAULT);
			VoC_lw16_p(REG3,REG1,DEFAULT);

			VoC_shr16_ri(REG2,1,DEFAULT);
			VoC_shr16_ri(REG3,1,IN_PARALLEL);

			VoC_add16_rr(REG4,REG2,REG3,DEFAULT);

			VoC_shr16_ri(REG4,-1,DEFAULT);

			VoC_NOP();

			VoC_sw16inc_p(REG4,REG1,DEFAULT);
			
		VoC_endloop0

		VoC_lw16_d(REG2,ENC_INPUT_ADDR_ADDR);

		VoC_lw16i(REG0,INPUT_SPEECH_BUFFER2_ADDR);

		VoC_lw32_d(REG45,GLOBAL_TX_ADDR);

		VoC_or16_ri(REG5,0x4000);

		VoC_be16_rr(CII_SPEECH_RECORD_L1,REG0,REG2)

			VoC_lw16i(ACC0_HI,320);
			VoC_shr32_ri(ACC0,16,DEFAULT);
			VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

CII_SPEECH_RECORD_L1:	
			
		VoC_shr16_ri(REG2,1,DEFAULT);
		VoC_lw16i(REG3,80);
		VoC_jal(Rda_RunDma);	
CII_SPEECH_RECORD_L0:


		VoC_pop32(REG67,DEFAULT);


CII_DECODER_RUN:


    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_and16_ri(REG7,2);

    VoC_lw16i(REG0, INPUT_BITS2_ADDR);

	VoC_bez16_r(CII_DECODER_L0,REG7)

		// SAVE PARAMETERS
		VoC_lw32inc_p(REG67,REG0,DEFAULT);//mode

		VoC_lw32inc_p(REG23,REG0,DEFAULT);
		VoC_lw32inc_p(REG45,REG0,DEFAULT);

		VoC_sw16_d(REG2,DEC_AMR_FRAME_TYPE_ADDR);

   		VoC_sw16_d(REG3,GLOBAL_BFI_ADDR);
		VoC_lw32inc_p(REG23,REG0,DEFAULT);

 		VoC_sw16_d(REG4,GLOBAL_SID_ADDR);
		VoC_sw16_d(REG5,GLOBAL_TAF_ADDR);

		VoC_sw16_d(REG2,GLOBAL_UFI_ADDR);


		VoC_jal(CII_MAIN_DECODER);


//		VoC_bnez16_d(CII_DECODER_OVER,AEC_ENABLE_FLAG_ADDR)
		//VoC_bnez16_d(CII_DECODER_L1,GLOBAL_ECHO_EC_MU)
		//VoC_bez16_d(CII_DECODER_OVER,GLOBAL_ECHO_ES_ON)
//CII_DECODER_L1:
// 			VoC_lw16i_short(REG0,0,DEFAULT);

//			VoC_bnez16_d(CII_DECODER_OVER,GLOBAL_BFI_ADDR)
//			VoC_bnez16_d(CII_DECODER_OVER,GLOBAL_SID_ADDR)

//  			VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,2);

			// **********************************
			// func: CII_ECHO_SUPPRESSION
			//
			// input : RL7     <- echo_rst
			//         REG7    <- echo_es_on

			//
			// output: ACC0_LO <- echo_vad_flag
			//         ACC1    <- enc_pwr
			//
			// used  : REG0123457
			//
			// unused: REG6
			// ***********************************

//			VoC_jal(CII_ECHO_SUPPRESSION);

//			VoC_lw16i_short(REG0,2,DEFAULT);
//		VoC_bez16_d(CII_DECODER_L3,GLOBAL_ECHO_EC_MU)
//			VoC_lw16i_short(REG0,0,DEFAULT);
//CII_DECODER_L3:
//
//		VoC_bnez16_d(CII_DECODER_OVER,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER)
//CII_DECODER_L2:
//
//		VoC_lw16i_short(REG3,4,DEFAULT);
//
//		VoC_bez16_d(CII_DECODER_L4,GLOBAL_ECHO_EC_MU)
//			VoC_lw16i_short(REG3,0,DEFAULT);
//CII_DECODER_L4:
//
//		VoC_add16_rr(REG3,REG3,REG0,DEFAULT);
//		
//		VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,2);
//		VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,2);		
//
//		VoC_lw32inc_p(REG45,REG1,DEFAULT);
//		VoC_shr16_rr(REG4,REG3,DEFAULT);
//		VoC_shr16_rr(REG5,REG3,IN_PARALLEL);
//
//
//		VoC_loop_i(0,(FRAMESIZE/2))
//			VoC_lw32inc_p(REG45,REG1,DEFAULT);
//			VoC_sw32inc_p(REG45,REG0,DEFAULT);				
//			VoC_shr16_rr(REG4,REG3,DEFAULT);
//			VoC_shr16_rr(REG5,REG3,IN_PARALLEL);								
// 		VoC_endloop0

//CII_DECODER_OVER:

	// scale receiver
	VoC_bez16_d(Rda_Encoder_Scale_Receiver,SCALE_RECEIVER_ENABLE_FLAG_ADDR)
		VoC_lw16d_set_inc(REG3, DEC_OUTPUT_ADDR_ADDR,1);
		VoC_lw16_d(REG0, GLOBAL_RECEIVER_SCALE_OUT_ADDR);
		VoC_lw16i_short(FORMAT32, 14, DEFAULT);
		VoC_loop_i(0, 160)
			VoC_multf32_rp(ACC0, REG0, REG3, DEFAULT);
			VoC_NOP();
			VoC_NOP();
			VoC_sw16inc_p(ACC0_LO, REG3, DEFAULT);
		VoC_endloop0
Rda_Encoder_Scale_Receiver:

	VoC_lw16_d(REG2,DEC_OUTPUT_ADDR_ADDR);

	VoC_lw16i(REG0,OUTPUT_SPEECH_BUFFER2_ADDR);

	VoC_lw32_d(REG45,GLOBAL_RX_ADDR);

	VoC_be16_rr(CII_MAIN_VOCODER_L1,REG0,REG2)
		VoC_lw16i(ACC0_HI,320);
		VoC_shr32_ri(ACC0,16,DEFAULT);
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);
CII_MAIN_VOCODER_L1:

	VoC_shr16_ri(REG2,1,DEFAULT);
    VoC_lw16i(REG3,80);
	VoC_jal(Rda_RunDma);
		

	// generate an interrupt if the flag is enabled
    VoC_lw16i_short(REG7,2,DEFAULT);
	VoC_and16_rd(REG7,GLOBAL_IRQGEN_ADDR);
	VoC_bez16_r(CII_DECODER_L0,REG7)
		VoC_cfg(CFG_CTRL_IRQ_TO_BCPU|CFG_CTRL_CONTINUE);
CII_DECODER_L0:


	VoC_lw16i_short(REG1,0xFFFF,DEFAULT);

	VoC_lw32z(ACC0,DEFAULT);

    VoC_be16_rd(CII_VOCODER_not_update_reset,REG1,(INPUT_BITS2_ADDR+1)) //mode = 0xFFFF :

		// clear the vocoder reset only if the mode is valid
		VoC_sw16_d(ACC0_LO,GLOBAL_VOCODER_RESET_ADDR);

CII_VOCODER_not_update_reset:
	
	// clear the general reset
	VoC_sw16_d(ACC0_LO,GLOBAL_RESET_ADDR);

	// clear msdf reload flag
	VoC_sw16_d(ACC0_LO,GLOBAL_MSDF_RELOAD_FLAG);

	VoC_lw32_d(REG45,ENC_INPUT_ADDR_ADDR);
    VoC_lw32_d(REG23,ENC_INPUT_ADDR_BAK_ADDR);
    VoC_sw32_d(REG45,ENC_INPUT_ADDR_BAK_ADDR);
	VoC_sw32_d(REG23,ENC_INPUT_ADDR_ADDR);

	VoC_jump(CII_VOCODER_EXIT);

}






void CII_MAIN_ENCODER(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_bez16_d(CII_MAIN_ENCODER_no_IF1,GLOBAL_IF1_flag_ADDR)     // if ( IF1 !=0 )  dtx = 0;
    VoC_lw16i_short(REG6,0,DEFAULT);

CII_MAIN_ENCODER_no_IF1:


	// ****************
	// function : CII_DigitalGain
	// input  : REG0 : old gain pointer,  INC0 = 2
	//          REG3 : work buffer,       INC3 = 2
	//          REG4 : new gain
	// output : none
	// used : REG01345
	// ****************

	//VoC_lw16i_set_inc(REG0,GLOBAL_OLD_MICGAIN_ADDR,2);
	VoC_NOP();
	VoC_sw16_d(REG6,GLOBAL_DTX_ADDR);

	//VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);
	//VoC_lw16_d(REG4,GLOBAL_ENC_MICGAIN_ADDR);

//	VoC_jal(CII_DigitalGain);

	// ****************
	// input :  REG2 : reset
	//          REG3 : coeffs_buffers, INC3 = 2
	// output : none
	// used : REG34, ACC0 1
	// not modified : REG2
	// ****************

	VoC_lw16i_set_inc(REG3,(GLOBAL_MDF_COEFFS_ADDR),2);
	VoC_lw32_d(ACC1,GLOBAL_MDF_ADDR);
	VoC_jal(CII_LoadSpFormatFilter);
	// ****************
	// input : REG0 : filter samples, INC0 = 2
	//         REG1 : out buffer,     INC1 = 2
	//         REG2 : filter coeffs,  INC2 = 2
	//         REG3 : source buffer,  INC3 = 2
	// output : none
	// used : REG012345
	// ****************
	VoC_lw16i_set_inc(REG0,GLOBAL_MDF_SAMPLS_ADDR,2);
	VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,2);
	VoC_lw16i_set_inc(REG2,GLOBAL_MDF_COEFFS_ADDR,2);
	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);

	VoC_bez16_d(CII_MAIN_ENCODER_SKIPFIR, GLOBAL_MICFIR_ENABLE_FLAG_ADDR)
		VoC_jal(CII_RunAntiDistorsionFilter);
CII_MAIN_ENCODER_SKIPFIR:

 
    VoC_lw16i_short(REG6,0xFFFF,DEFAULT);

	// bypass in calibration mode
   	VoC_be16_rr(CII_MAIN_ENCODER_end,REG7,REG6);
		// reload encoder
		VoC_jal(CII_reload_mode);

// VoC_directive: PARSER_OFF

		// main function call reset part in it
		VoC_bnlt16_rd(CII_MAIN_ENCODER101,REG7,CONST_0x1fff_ADDR); //0x80
			VoC_jal(CII_AMR_Encode);
			VoC_jump(CII_MAIN_ENCODER_end);
CII_MAIN_ENCODER101:
		VoC_bne16_rd(CII_MAIN_ENCODER102,REG7,CONST_0x1fff_ADDR);
			VoC_jal(CII_FR_Encode);
			VoC_jump(CII_MAIN_ENCODER_end);
CII_MAIN_ENCODER102:

// VoC_directive: PARSER_ON

   		// case is no needed in binary simulation
		// because all functions have the same start address
		VoC_jal(CII_HR_Encode);

CII_MAIN_ENCODER_end:

	// output parameters

    VoC_movreg16(REG7,REG6,DEFAULT);
    VoC_lw16_d(REG6,GLOBAL_RELOAD_MODE_ADDR);
  
	VoC_lw16_d(REG3,GLOBAL_OUTPUT_SP_ADDR);			//sp_flag
	VoC_sw32_d(REG67,OUTPUT_BITS2_ADDR);
  
   	VoC_sw16_d(REG3,OUTPUT_BITS2_ADDR+3);
	VoC_pop16(RA,DEFAULT);

	// generate an interrupt if the flag is enabled
    VoC_lw16i_short(REG7,1,DEFAULT);
	VoC_and16_rd(REG7,GLOBAL_IRQGEN_ADDR);
	VoC_bez16_r(no_irqgen_enc,REG7)
		VoC_cfg(CFG_CTRL_IRQ_TO_BCPU|CFG_CTRL_CONTINUE);
no_irqgen_enc:



	VoC_return
}



// VoC_directive: ALIGN

void CII_MAIN_DECODER(void)
{

	VoC_push16(RA,DEFAULT);

    VoC_bez16_d(CII_MAIN_DECODER_NO_IF1,GLOBAL_IF1_flag_ADDR)

    VoC_lw16_p(REG3,REG0,DEFAULT);   //load the header of IF1 frame

	VoC_lw16i_short(REG0,4,DEFAULT);
	VoC_movreg16(REG5,REG3,IN_PARALLEL);

	VoC_and16_rr(REG0,REG3,DEFAULT);
	
    VoC_and16_ri(REG3,0x38);

	VoC_and16_ri(REG5,0xc3);

    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_bez16_r(CII_MAIN_DECODER_Good_IF1_frame,REG0)
	VoC_bnez16_r(CII_MAIN_DECODER_Good_IF1_frame,REG5)
    VoC_lw16i_short(REG2,0,DEFAULT);
CII_MAIN_DECODER_Good_IF1_frame:
	
	VoC_shr16_ri(REG3,3,DEFAULT);   //mode
    VoC_sw16_d(REG2,DEC_AMR_FRAME_TYPE_ADDR);
    VoC_and16_ri(REG7,0xf0);
    VoC_or16_rr(REG7,REG3,DEFAULT);  //update the decode mode
  
CII_MAIN_DECODER_NO_IF1:


	// ****************
	// input :  REG2 : reset
	//          REG3 : coeffs_buffers, INC3 = 2
	// output : none
	// used : REG34, ACC0 1
	// not modified : REG2
	// ****************


	VoC_lw16i_set_inc(REG3,(GLOBAL_SDF_COEFFS_ADDR),2);
	VoC_lw32_d(ACC1,GLOBAL_SDF_ADDR);
	VoC_jal(CII_LoadSpFormatFilter);

	VoC_lw16d_set_inc(REG3,(DEC_OUTPUT_ADDR_ADDR),2);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_short(REG6,0xFFFF,IN_PARALLEL);

	VoC_loop_i(0,80);
		VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0


	// bypass in calibration mode
   	VoC_be16_rr(CII_MAIN_DECODER_end,REG7,REG6);
		// reload decoder
		VoC_jal(CII_reload_mode);

        VoC_lw16i(REG6,0x177);
		// main function call reset part in it
		VoC_bgt16_rr(CII_MAIN_DECODER102,REG7,REG6);
			VoC_jal(CII_AMR_Decode);
			VoC_jump(CII_MAIN_DECODER_end);
CII_MAIN_DECODER102:
		VoC_bne16_rd(CII_MAIN_DECODER104,REG7,CONST_0x1fff_ADDR);
			VoC_jal(CII_FR_Decode);
			VoC_jump(CII_MAIN_DECODER_end);
CII_MAIN_DECODER104:
			VoC_jal(CII_HR_Decode);


CII_MAIN_DECODER_end:

	VoC_bez16_d(Rda_Receiver_NoiseSuppress,RECEIVER_NOISE_SUPPRESSER_ENABLE_FLAG_ADDR)
		VoC_jal(Rda_SpeexReceiverNoiseSuppresser);
Rda_Receiver_NoiseSuppress:

	// agc, nonclip, eq all in agc.tab
	VoC_bnez16_d(Rda_DecoderLoadAgcTab,RECEIVER_NONCLIP_ENABLE_FLAG_ADDR)
	VoC_bnez16_d(Rda_DecoderLoadAgcTab,STRUCT_ReceiverAgcConfig_enable_addr)
	VoC_bez16_d(Rda_DecoderSKIPAgcTab,IIR_ENABLE_FLAG_ADDR)
Rda_DecoderLoadAgcTab:
		VoC_jal(Rda_ReceiverAgc);
Rda_DecoderSKIPAgcTab:

	// ****************
	// input : REG0 : filter samples, INC0 = 2
	//         REG1 : target buffer,  INC1 = 2
	//         REG2 : filter coeffs,  INC2 = 2
	//         REG3 : source buffer,  INC3 = 2
	// output : none
	// used : REG012345
	// ****************

	VoC_lw16i_set_inc(REG0,GLOBAL_SDF_SAMPLS_ADDR,2);
	VoC_lw16d_set_inc(REG1,DEC_OUTPUT_ADDR_ADDR,2);
	VoC_lw16i_set_inc(REG2,GLOBAL_SDF_COEFFS_ADDR,2);
	VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,2);

	// ****************
	// input : REG0 : filter samples, INC0 = 2
	//         REG1 : target buffer,  INC1 = 2
	//         REG2 : filter coeffs,  INC2 = 2
	//         REG3 : source buffer,  INC3 = 2
	// output : none
	// used :
	// ****************

	VoC_bez16_d(CII_MAIN_DECODER_SKIPFIR, GLOBAL_RECEIVERFIR_ENABLE_FLAG_ADDR)
		VoC_jal(CII_RunAntiDistorsionFilter);
CII_MAIN_DECODER_SKIPFIR:

	// load source and destination pointers
//	VoC_lw16i_set_inc(REG0,GLOBAL_OLD_SPKGAIN_ADDR,2);
//	VoC_lw16d_set_inc(REG3,DEC_OUTPUT_ADDR_ADDR,2);
 //   VoC_lw16_d(REG4,GLOBAL_DEC_SPKGAIN_ADDR);

	// ****************
	// function : CII_DigitalGain
	// input  : REG0 : old gain pointer,  INC0 = 2
	//          REG3 : work buffer,       INC3 = 2
	//          REG4 : new gain
	// output : none
	// used : REG01345
	// ****************

//	VoC_jal(CII_DigitalGain);

	VoC_pop16(RA,DEFAULT);


	VoC_NOP();
	VoC_return;

}

// ****************************
// func  : CII_ECHO_MAIN
//
//
// output: REG6 <- echo_vad
//         REG7 <- func_id = 2
//         ACC0 <- enc_corr_pwr
//         ACC1 <- enc_pwr
//
// used  : all
// ****************************


// VoC_directive: PARSER_OFF
#if 0
#define echo_format32       6  // format for the pwr computation

void CII_ECHO_MAIN(void)
{

	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(REG6,0,IN_PARALLEL);

	// at this point the counter = 0, 1 or 2
	VoC_bngtz16_d(no_mic_muting_begin,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER)

			// echo vad state counter
		VoC_lw16d_set_inc(REG3,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER,-1);
		VoC_lw16i_short(REG6,1,DEFAULT);
		// at this point the counter = 1 or 2
		// decrement counter (INC3 = -1)
		VoC_inc_p(REG3,DEFAULT);
		// echo vad state counter
		VoC_sw16_d(REG3,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER);
no_mic_muting_begin:

    // no mute if es_on = 0, return
//	VoC_lw16_d(REG1,GLOBAL_ECHO_EC_MU);
//	VoC_bez16_r(CII_ECHO_MAIN_L3 ,REG1)

		VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,2);

		// compute en_in PWR
		VoC_lw32z(ACC0,DEFAULT);
		VoC_lw16i_short(FORMAT32,echo_format32,DEFAULT);

		VoC_loop_i(0,80)
			VoC_bimac32inc_pp(REG0,REG0);
		VoC_endloop0;

		
		VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);

		VoC_lw32z(ACC0,DEFAULT);
		VoC_lw16i_short(FORMAT32,echo_format32,IN_PARALLEL);

		VoC_sw32_d(ACC0,GLOBAL_OUTPUTENERGY_ADDR);

		VoC_loop_i(0,80)
			VoC_bimac32inc_pp(REG0,REG0);
		VoC_endloop0;

		VoC_NOP();

		VoC_movreg16(REG1,ACC0_HI,DEFAULT);
		VoC_lw16i_short(FORMAT32,-16,IN_PARALLEL);

		VoC_multf32_rd(ACC0,REG1,GLOBAL_ECHO_EC_REL);

		VoC_sw32_d(ACC0,GLOBAL_INPUTENERGY_ADDR);

		VoC_blt32_rd(CII_ECHO_MAIN_L1,ACC0,GLOBAL_OUTPUTENERGY_ADDR)

			VoC_lw16i_short(REG6,0,DEFAULT);

	CII_ECHO_MAIN_L1:

/*

		VoC_lw16i(ACC0_LO,0x0000);
		VoC_lw16i(ACC0_HI,0x0030);

		VoC_lw32_d(ACC1,GLOBAL_INPUTENERGY_ADDR);

		VoC_bez16_d(CII_ECHO_MAIN_L2,NOISE_SUPPRESSER_WITHOUT_SPEECH_ENABLE_FLAG_ADDR)

		VoC_bgt32_rr(CII_ECHO_MAIN_L2,ACC1,ACC0)

			VoC_lw16i_short(REG1,1,DEFAULT);
			VoC_add16_rd(REG1,REG1,GLOBAL_MIC_ENERGY_COUNTER_ADDR);
			VoC_lw16i_short(REG2,10,DEFAULT);
			VoC_sw16_d(REG1,GLOBAL_MIC_ENERGY_COUNTER_ADDR);

			VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);

			VoC_bgt16_rr(CII_ECHO_MAIN_L3,REG2,REG1)

				VoC_lw16i_short(REG1,26,DEFAULT);
				VoC_lw16i_short(FORMAT16,4-16,IN_PARALLEL);
				
				VoC_sub16_rd(REG1,REG1,GLOBAL_MIC_ENERGY_COUNTER_ADDR);

				VoC_lw16i_short(REG5,4,DEFAULT);

				VoC_bgt16_rr(CII_ECHO_MAIN_L4,REG1,REG5);
					VoC_lw16i_short(REG1,4,DEFAULT);
CII_ECHO_MAIN_L4:

				VoC_loop_i(0,80)
					VoC_lw32_p(REG23,REG0,DEFAULT);

					VoC_multf16_rr(REG2,REG2,REG1,DEFAULT);
					VoC_multf16_rr(REG3,REG3,REG1,IN_PARALLEL);
					
					VoC_NOP();
					VoC_NOP();
					VoC_sw32inc_p(REG23,REG0,DEFAULT);
					
				VoC_endloop0;

				VoC_jump(CII_ECHO_MAIN_L3);

	CII_ECHO_MAIN_L2:

			VoC_sw16_d(ACC0_LO,GLOBAL_MIC_ENERGY_COUNTER_ADDR);

	CII_ECHO_MAIN_L3:

*/

    // no mute if es_on = 0, return	
	VoC_bnez16_d(LOUDSPEAKER_no_mic_muting_end ,GLOBAL_ECHO_ES_ON)

	VoC_lw16_d(REG1,GLOBAL_ECHO_EC_MU);
	VoC_bez16_r(NORMAL_no_mic_muting_end ,REG1)

LOUDSPEAKER_no_mic_muting_end:

	VoC_bez16_r(NORMAL_no_mic_muting_lastframe,REG6)
	
		VoC_bnez16_d(NORMAL_no_mic_muting_nofirstframe,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR)
			VoC_lw16i_short(REG2,1,DEFAULT);
			VoC_lw16i_set_inc(REG2,GLOBAL_COMFORTALBE_NOISE_ADDR,1);
			VoC_sw16_d(REG2,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR);
			VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);
			VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,1);

			VoC_lw16i_short(FORMAT16,7-16,DEFAULT);
			VoC_lw16i_short(REG0,0,DEFAULT);
			VoC_lw16i_short(REG5,127,IN_PARALLEL);
			VoC_lw16i_short(INC0,1,DEFAULT);

			VoC_loop_i(0,128)
				VoC_multf16inc_rp(REG4,REG0,REG2,DEFAULT);
				VoC_sub16_rr(REG7,REG5,REG0,IN_PARALLEL);

				VoC_multf16inc_rp(REG6,REG7,REG3,DEFAULT);
				VoC_inc_p(REG0,DEFAULT);

				VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
				VoC_NOP();
				VoC_sw16inc_p(REG4,REG1,DEFAULT);				
			VoC_endloop0;

			VoC_loop_i(0,32)
				VoC_multf16inc_rp(REG4,REG0,REG2,DEFAULT);
				VoC_sub16_rr(REG7,REG5,REG0,IN_PARALLEL);

				VoC_multf16inc_rp(REG6,REG7,REG3,DEFAULT);
				VoC_NOP();

				VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
				VoC_NOP();
				VoC_sw16inc_p(REG4,REG1,DEFAULT);				
			VoC_endloop0;

		VoC_jump(NORMAL_no_mic_muting_end);

NORMAL_no_mic_muting_nofirstframe: 

		VoC_lw16i_short(REG2,1,DEFAULT);
		VoC_lw16i_set_inc(REG2,GLOBAL_COMFORTALBE_NOISE_ADDR,2);
		VoC_sw16_d(REG2,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR);
		VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);
		VoC_lw32inc_p(ACC0,REG2,DEFAULT);
		VoC_loop_i(0,80)
			VoC_lw32inc_p(ACC0,REG2,DEFAULT);
			VoC_sw32inc_p(ACC0,REG3,DEFAULT);				
		VoC_endloop0;
		VoC_jump(NORMAL_no_mic_muting_end);


NORMAL_no_mic_muting_lastframe:

	VoC_bez16_d(NORMAL_no_mic_muting_end,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR)
		VoC_lw16i_short(REG2,0,DEFAULT);
		VoC_lw16i_set_inc(REG2,GLOBAL_COMFORTALBE_NOISE_ADDR,1);
		VoC_sw16_d(REG2,GLOBAL_ECHO_ON_INLOUDSPEAKER_FLAG_ADDR);
		VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);
		VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,1);

		VoC_lw16i_short(FORMAT16,7-16,DEFAULT);
		VoC_lw16i_short(REG0,0,DEFAULT);
		VoC_lw16i_short(REG5,127,IN_PARALLEL);
		VoC_lw16i_short(INC0,1,DEFAULT);

		VoC_loop_i(0,128)
			VoC_multf16inc_rp(REG4,REG0,REG3,DEFAULT);
			VoC_sub16_rr(REG7,REG5,REG0,IN_PARALLEL);

			VoC_multf16inc_rp(REG6,REG7,REG2,DEFAULT);
			VoC_inc_p(REG0,DEFAULT);

			VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
			VoC_NOP();
			VoC_sw16inc_p(REG4,REG1,DEFAULT);				
		VoC_endloop0;
NORMAL_no_mic_muting_end:

    VoC_pop16(RA,DEFAULT);
	// format32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
	// fct_id = echo flag = 2
//	VoC_lw16i_short(REG7,2,DEFAULT);
	VoC_return;

}


// **********************************
// func: CII_ECHO_SUPPRESSION
//
// input :

//
// output: ACC0_LO <- echo_vad_flag
//         ACC1    <- enc_pwr
//
// used  : REG0123457
//
// unused: REG6
// ***********************************


void CII_ECHO_SUPPRESSION(void)
{

	// push RA
	VoC_push16(RA,DEFAULT);
	VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

	// compute dec_out PWR
	VoC_lw32z(ACC0,DEFAULT);
	VoC_lw16i_short(INC0,2,IN_PARALLEL);

	VoC_loop_i(0,80)
		VoC_bimac32inc_pp(REG0,REG0);
	VoC_endloop0;

	/**************************/
	// input in ACC0
	// output in REG1  only used ACC0, REG1
	/**************************/

	VoC_lw16i_short(REG1,31,DEFAULT);
	VoC_bez32_r(dec_pwr_null,ACC0)

	// get 32 - log(dec_pwr) in REG1
	VoC_jal(CII_NORM_L_ACC0);

dec_pwr_null:

	// echo vad state counter
	VoC_lw16d_set_inc(REG3,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER,-1);

	// no mute if es_on = 0, return
	VoC_bez16_d(LOUDSPEAKER_MODE_no_echo_VAD_counter_reset,GLOBAL_ECHO_ES_ON)

	// no mute if 32 - log(dec_pwr) > echo_vad_thr  (no voice activity on decoder)
	VoC_bgt16_rd(LOUDSPEAKER_MODE_no_echo_VAD_counter_reset,REG1,GLOBAL_ECHO_ES_VAD)

		// reset counter
		VoC_lw16i_short(REG3,4,DEFAULT);
	
	VoC_jump(NORMAL_MODE_no_echo_VAD_counter_reset);

LOUDSPEAKER_MODE_no_echo_VAD_counter_reset:


	VoC_bez16_d(NORMAL_MODE_no_echo_VAD_counter_reset,GLOBAL_ECHO_EC_MU)

	// no mute if 32 - log(dec_pwr) > echo_vad_thr  (no voice activity on decoder)
	VoC_bgt16_rd(NORMAL_MODE_no_echo_VAD_counter_reset,REG1,GLOBAL_ECHO_EC_MIN)

		// reset counter
		VoC_lw16i_short(REG3,4,DEFAULT);

NORMAL_MODE_no_echo_VAD_counter_reset:

	// RA
	VoC_pop16(RA,DEFAULT);

	// echo vad state counter
	VoC_sw16_d(REG3,GLOBAL_ECHO_SUPPRESS_EXP_COUNTER);

	VoC_return;
}

#endif
// VoC_directive: PARSER_ON

/**************************/
// input in ACC0
// output in REG1  only used ACC0, REG1
/**************************/

void CII_NORM_L_ACC0(void)
{

	VoC_lw16i_set_inc(REG1,0,1);
	VoC_push32(ACC1,DEFAULT);
	VoC_movreg32(ACC1,ACC0,IN_PARALLEL);
	VoC_bez32_r(G_PITCH_NORM_L_3_EXIT,ACC0)
	VoC_bnltz32_r(G_PITCH_NORM_L_3_1,ACC0)
		VoC_not32_r(ACC0,DEFAULT);
G_PITCH_NORM_L_3_1:
		VoC_loop_i_short(31,DEFAULT)
		VoC_startloop0
			VoC_bnlt32_rd(G_PITCH_NORM_L_3_EXIT,ACC0,CONST_0x40000000_ADDR)
			VoC_shr32_ri(ACC0,-1,DEFAULT);
			VoC_inc_p(REG1,IN_PARALLEL);
			VoC_shr32_ri(ACC1,-1,DEFAULT);
		VoC_endloop0
G_PITCH_NORM_L_3_EXIT:
	VoC_movreg32(ACC0,ACC1,DEFAULT);
	VoC_pop32(ACC1,DEFAULT);
	VoC_return
}



/*********************/
// input in REG0, REG1
// output in  REG2
// used register RL6, RL7
// REG0/REG1
/*********************/

void CII_DIV_S(void)
{
	VoC_lw16i(REG2,0x7fff);
	VoC_be16_rr(SCHUR1_DIV_S_11,REG1,REG0)
	VoC_lw16i_set_inc(REG2,0,1);

	VoC_movreg16(RL6_LO, REG0, DEFAULT);
	VoC_movreg16(RL7_LO, REG1, IN_PARALLEL);
	VoC_lw16i_short(RL6_HI,0,DEFAULT);
	VoC_lw16i_short(RL7_HI,0, IN_PARALLEL);

	VoC_loop_i_short(14,DEFAULT)
	VoC_shr32_ri(RL6,-1,IN_PARALLEL);
	VoC_startloop0

		VoC_bgt32_rr(SCHUR1_DIV_S_1,RL7,RL6)
		VoC_sub32_rr(RL6,RL6,RL7,DEFAULT);
		VoC_inc_p(REG2,IN_PARALLEL);
	SCHUR1_DIV_S_1:
		VoC_shr16_ri(REG2,-1,DEFAULT);
		VoC_shr32_ri(RL6,-1,IN_PARALLEL);
		VoC_endloop0
	VoC_bgt32_rr(SCHUR1_DIV_S_11,RL7,RL6)
	VoC_inc_p(REG2,DEFAULT);
SCHUR1_DIV_S_11:

	VoC_return
}


// ****************
// input :  REG2 : reset
//          REG3 : coeffs_buffers, INC3 = 2
// output : none
// used : REG34, ACC0 1
// not modified : REG2
// ****************

void CII_LoadSpFormatFilter(void)
{
    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
	VoC_bnez16_d(MSDF_RELOAD_ENABLE,GLOBAL_RESET_ADDR)
	VoC_bez16_d(DONE_MSDF,GLOBAL_MSDF_RELOAD_FLAG)
MSDF_RELOAD_ENABLE:
	// if pointer == NULL, apply ID filter
	VoC_bnez32_r(LOAD_MSDF,ACC1);

		VoC_lw32z(RL7,DEFAULT);
		VoC_lw32_d(ACC1,CONST_0x40000000_ADDR);

		VoC_loop_i_short((SECTION_MSDF_SIZE-1),DEFAULT)
		VoC_startloop0
		    VoC_sw32inc_p(RL7,REG3,DEFAULT);
		VoC_endloop0;

		VoC_sw32_p(ACC1,REG3,DEFAULT);

		VoC_jump(DONE_MSDF);

LOAD_MSDF:
		// configure addresses of burst
        VoC_shr16_ri(REG3,1,DEFAULT);
 		VoC_cfg_lg(SECTION_MSDF_SIZE,CFG_DMA_SIZE);
      	// store local address
		VoC_sw16_d(REG3,CFG_DMA_LADDR);  // local address
		// store extern address
		VoC_sw32_d(ACC1,CFG_DMA_EADDR); // read non-single access

		VoC_cfg(CFG_CTRL_STALL);

		VoC_NOP();
		// clear the event
		VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

DONE_MSDF:
   	VoC_NOP();
	VoC_return;
}


void CII_encoder_homing_frame_test(void)
{
	VoC_lw16i_short(ACC0_LO,0x0008,DEFAULT);
	VoC_lw16i_short(REG4,0,DEFAULT);
	VoC_movreg16(ACC0_HI,ACC0_LO,IN_PARALLEL);

	VoC_loop_i(0,80)
		VoC_lw32inc_p(REG67,REG0,DEFAULT);
	   	VoC_bne32_rr(CII_encoder_homing_frame_test_01,REG67,ACC0)
	VoC_endloop0

	VoC_lw16i_short(REG4,1,DEFAULT);

CII_encoder_homing_frame_test_01:

	VoC_return

}




// ****************************
// func: CII_reload_sideinfo
// input : REG7 <- mode flag
//         REG5     enc : 2 ,  dec : 1
// output: -
// used : REG3467
// ****************************

void CII_reload_sideinfo(void)
{

	// ******************
	// RESET
	// ******************

	// new & old reload mode
	VoC_movreg16(REG6,REG7,DEFAULT);
	VoC_lw16_d(REG4,GLOBAL_RELOAD_MODE_ADDR);

	// ignore AMR mode bits
	VoC_shr16_ri(REG6,7,DEFAULT);
	VoC_shr16_ri(REG4,7,IN_PARALLEL);

	// compute reset flag
	VoC_lw16i_short(REG2,0,DEFAULT);

	VoC_be16_rr(NO_SECTION_RESET,REG6,REG4);
	VoC_lw16i_short(REG2,1,DEFAULT);
NO_SECTION_RESET:

	// reset if vocoder reload is detected now or during the previous turn


	VoC_lw16i_short(REG4,8,DEFAULT);
   	VoC_or16_rd(REG2,GLOBAL_VOCODER_RESET_ADDR);

	VoC_sw16_d(REG7,GLOBAL_RELOAD_MODE_ADDR);
    VoC_lw16i_set_inc(REG3,GLOBAL_DEC_MODE_ADDR,1);

	// keep reset flag & new mode for next turn
	VoC_sw16_d(REG2,GLOBAL_VOCODER_RESET_ADDR);




	// ******************
	// AMR MODE FLAGS
	// ******************

	// mode flag for AMR modes
	VoC_loop_i_short(2,DEFAULT)

	VoC_startloop0

		VoC_lw16i_short(REG6,0xf,DEFAULT);
		VoC_and16_rr(REG6,REG7,DEFAULT);

		// check if a section was reloaded
        VoC_be16_rr(SECTION_4_DONE,REG6,REG4)
			// mode flags
           	VoC_sw16inc_p(REG6,REG3,DEFAULT);
	//	   	VoC_sw16inc_p(REG6,REG3,DEFAULT);

SECTION_4_DONE:

		VoC_shr16_ri(REG7,4,DEFAULT);

         VoC_lw16i_set_inc(REG3,GLOBAL_ENC_MODE_ADDR,1);
	VoC_endloop0;

    VoC_sw16_d(REG6,GLOBAL_ENC_USED_MODE_ADDR);


	// ******************
	// EFR MODE FLAG
	// ******************

	// mode flag for EFR
	VoC_lw16i_short(REG6,0xf,DEFAULT);
	VoC_and16_rr(REG6,REG7,DEFAULT);
    VoC_lw16i_short(REG3,15,IN_PARALLEL);
	// check if a section was reloaded
//	VoC_be16_rd(SECTION_2_DONE,REG6,CONST_15_ADDR)
	VoC_be16_rr(SECTION_2_DONE,REG6,REG3)
   		VoC_sw16_d(REG6,GLOBAL_EFR_FLAG_ADDR);
SECTION_2_DONE:


	VoC_return;

}




void CII_reload_mode(void)
{

	// AMR (decoder mode i) : 0x00fj
	// AMR (encoder mode j) : 0x00jf

	// EFR                  : 0x0177
	// FR                   : 0x1fff
	// HR                   : 0x2fff

	// Bypass               : 0xffff
	// f <=> don't care

	VoC_push16(REG7,DEFAULT);
	VoC_push16(RA,DEFAULT);

	// ******************
	// SIDE INFO
	// ******************

	VoC_jal(CII_reload_sideinfo);
	VoC_lw16_sd(REG7,1,DEFAULT);

    VoC_push16(REG2,DEFAULT);
	VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_lw16i_short(REG2,0,DEFAULT);
	// ******************
	// AMR DECODER SECTION
	// ******************

	VoC_lw16i(REG4, SECTION_AMR_DEC_SIZE );			// transfer size
	VoC_lw16i(REG5,SECTION_4_START);							// local addr
	VoC_lw16i(REG6,GLOBAL_LOADED_AMR_DEC_MODE_ADDR);			// &curr_mode
	VoC_lw16i(REG3,GLOBAL_AMR_DEC_MODE_SOURCE_CODE_ADDR_ADDR);  // reload table



	// ****************************
	// func: CII_reload_section
	// input : REG3 <- reload table ptr
	//         REG4 <- mode info ptr
	//         REG5 <- local section ptr
	//         REG6 <- transfer size
	//         REG7 <- mode flag
	// output: REG1 <- new mode (if no reload = 0xf)
	// used : REG1345
	// ****************************

	// reload section 1
	VoC_jal(CII_reload_section);

	// ******************
	// AMR ENCODER SECTION
	// ******************

	VoC_lw16i(REG4,SECTION_AMR_ENC_SIZE );		// transfer size
	VoC_lw16i(REG5,SECTION_3_START);							// local addr
	VoC_lw16i(REG6,GLOBAL_LOADED_AMR_ENC_MODE_ADDR);			// &curr_mode
	VoC_lw16i(REG3,GLOBAL_AMR_ENC_MODE_SOURCE_CODE_ADDR_ADDR);  // reload table


	// ****************************
	// func: CII_reload_section
	// input : REG3 <- reload table ptr
	//         REG4 <- mode info ptr
	//         REG5 <- local section ptr
	//         REG6 <- transfer size
	//         REG7 <- mode flag
	// output: REG1 <- new mode (if no reload = 0xf)
	// used : REG1345
	// ****************************

	// reload section 1
	VoC_jal(CII_reload_section);

	// ******************
	// AMR or EFR SECTION
	// ******************

	VoC_lw16i(REG4,SECTION_AMR_SPE_SIZE );	// transfer size
	VoC_lw16i(REG5,SECTION_2_START);						// local addr
	VoC_lw16i(REG6,GLOBAL_LOADED_AMR_EFR_ADDR);				// &curr_amr_or_efr
	VoC_lw16i(REG3,GLOBAL_AMR_EFR_SOURCE_CODE_ADDR_ADDR);	// &reload table

	// ****************************
	// func: CII_reload_section
	// input : REG3 <- reload table ptr
	//         REG4 <- mode info ptr
	//         REG5 <- local section ptr
	//         REG6 <- transfer size
	//         REG7 <- mode flag
	// output: REG1 <- new mode (if no reload = 0xf)
	// used : REG1345
	// ****************************

	// reload section 2
	VoC_jal(CII_reload_section);

	// ******************
	// VOCODER SECTION
	// ******************

   	VoC_lw16i_short(REG0,3,DEFAULT);

	// reload vocoder FR, HR
	VoC_lw16i(REG4,(SECTION_2_START-SECTION_1_START));	// transfer size for EFR or AMR

	VoC_lw16i(ACC0_LO,SECTION_AMR_EFR_RAMX_CONTANTS_SIZE);
    VoC_lw16i(ACC0_HI,AMR_EFR_RAMX_CONSTANTS_ADDR/2);

	VoC_lw16i(ACC1_LO,SECTION_AMR_EFR_RAMY_CONTANTS_SIZE);
    VoC_lw16i(ACC1_HI,AMR_EFR_RAMY_CONSTANTS_ADDR/2);

	VoC_lw16i_set_inc(REG2,GLOBAL_CONSTANTS_ADDR_ADDR,2);

	VoC_bez16_r(AMR_EFR_COMMON_LOAD,REG7)

	VoC_lw16i(REG4, SECTION_VOCODER_SIZE );	// transfer size for the rest

	VoC_lw16i(ACC0_LO,SECTION_HR_RAMX_CONTANTS_SIZE);
    VoC_lw16i(ACC0_HI,HR_RAMX_CONSTANTS_ADDR/2);

	VoC_lw16i(ACC1_LO,SECTION_HR_RAMY_CONTANTS_SIZE);
    VoC_lw16i(ACC1_HI,HR_RAMY_CONSTANTS_ADDR/2);

	VoC_lw16i_set_inc(REG2,GLOBAL_CONSTANTS_ADDR_ADDR+4,2);
AMR_EFR_COMMON_LOAD:


	VoC_lw16i(REG5,SECTION_1_START);			// local addr
	VoC_lw16i(REG6,GLOBAL_LOADED_VOCODER_ADDR);		// &curr_vocoder
	VoC_lw16i(REG3,GLOBAL_VOCODER_SOURCE_CODE_ADDR_ADDR);	// &reload table


	// ****************************
	// func: CII_reload_section
	// input : REG3 <- reload table ptr
	//         REG4 <- mode info ptr
	//         REG5 <- local section ptr
	//         REG6 <- transfer size
	//         REG7 <- mode flag
	// output: REG1 <- new mode (if no reload = 0xf)
	// used : REG1345
	// ****************************

	// reload section 1
	VoC_jal(CII_reload_section);

    VoC_pop16(REG2,DEFAULT);
	VoC_pop16(RA,DEFAULT);
	VoC_pop16(REG7,DEFAULT);
	VoC_return;

}


void CII_DMA_READ(void)
{
    VoC_sw16_d(REG5,CFG_DMA_LADDR);    // store local
    VoC_sw16_d(REG4,CFG_DMA_SIZE);  // get extern
	VoC_sw32_d(RL6,CFG_DMA_EADDR); // read non-single access

	VoC_cfg(CFG_CTRL_STALL);

	VoC_NOP();
	// clear the event
	VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

// VoC_directive: PARSER_OFF
    printf("Copying %d bursts of 4 from extern %08x to local %04x address\n",REGS[REG4].reg,REGL[2],REGS[REG5].reg);
// VoC_directive: PARSER_ON

	VoC_return;

}


// ****************************
// func: CII_prepare_reload
// input : REG1 <- new mode
//         REG3 <- reload table ptr
//         REG4 <- mode info ptr
//         REG5 <- local section ptr
//         REG0 <- load counter
// output : none
// used : REG1345
// ****************************


void CII_reload_section(void)
{
	// section flag
	VoC_lw16i_short(REG1,0xf,DEFAULT);
	VoC_and16_rr(REG1,REG7,DEFAULT);
	VoC_shr16_ri(REG7,4,DEFAULT);

	// curr_mode in REG6
	VoC_lw16_p(REG6,REG6,IN_PARALLEL);

	// update mode info
	VoC_sw16_p(REG1,REG6,DEFAULT);

	// check if reload is necessary

	VoC_be16_rd(NO_RELOAD,REG1,CONST_15_ADDR)
	VoC_be16_rr(NO_RELOAD,REG1,REG6)

		// &reload_table_addr[mode]
		VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
		VoC_add16_rr(REG3,REG3,REG1,DEFAULT);

       VoC_push16(RA,DEFAULT);
	   VoC_lw32_p(RL6,REG3,DEFAULT);             // get extern

       VoC_loop_r_short(REG0,DEFAULT);
	   VoC_startloop0
       VoC_jal(CII_DMA_READ);
       VoC_movreg32(REG45,ACC0,DEFAULT);
       VoC_movreg32(ACC0,ACC1,IN_PARALLEL);
	   VoC_lw32inc_p(RL6,REG2,DEFAULT);
       VoC_endloop0

       VoC_pop16(RA,DEFAULT);
	   VoC_NOP();

NO_RELOAD:
		VoC_return;

}




// ****************
// input : REG0 : filter samples, INC0 = 2
//         REG3 : source buffer,  INC3 = 2
//         REG4 : nb_tap
// output : none
// used : REG02345, ACC01
// ****************


void CII_shift_filter (void)
{

	VoC_push16(REG0,DEFAULT);
	VoC_lw16i(REG2,80);
//	VoC_sub16_dr(REG5,CONST_80_ADDR,REG4);
   VoC_sub16_rr(REG5,REG2,REG4,DEFAULT);
	// pointers
	VoC_lw16i_set_inc(REG2,MIC_FILT_ADDR,2);    // in RAM_X

	// copy old frame values in the temp buffer
	VoC_loop_r_short(REG4,DEFAULT)
	VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
	VoC_startloop0
		VoC_lw32inc_p(ACC0,REG0,DEFAULT);
exit_on_warnings = OFF;
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
exit_on_warnings = ON;
	VoC_endloop0

	// copy new frame values in the temp buffer
	VoC_loop_r_short(REG5,DEFAULT)
	VoC_lw32inc_p(ACC0,REG3,IN_PARALLEL);
	VoC_startloop0
		VoC_lw32inc_p(ACC0,REG3,DEFAULT);
exit_on_warnings = OFF;
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
exit_on_warnings = ON;
	VoC_endloop0

	// continue copy new frame values in the temp buffer
	// save last values of the new frame
	VoC_pop16(REG0,DEFAULT);

	VoC_loop_r_short(REG4,DEFAULT);
	VoC_startloop0
		VoC_movreg32(ACC1,ACC0,DEFAULT);
		VoC_lw32inc_p(ACC0,REG3,DEFAULT);

exit_on_warnings = OFF;
		VoC_sw32inc_p(ACC0,REG2,DEFAULT);
exit_on_warnings = ON;
		VoC_sw32inc_p(ACC1,REG0,DEFAULT);
	VoC_endloop0

	VoC_return;

}




// ****************
// input : REG0 : filter samples, INC0 = 2
//         REG1 : out buffer,     INC1 = 2
//         REG2 : filter coeffs,  INC2 = 2
//         REG3 : source buffer,  INC3 = 2
// output : none
// used :
// ****************


void CII_RunAntiDistorsionFilter (void)
{
	// ---------------------------------
	// copy and set the appropriate gain
	// gain 1.0 <=> 0x4000
	// ---------------------------------

	VoC_push16(RA,DEFAULT);
	VoC_push16(REG2,DEFAULT);
	VoC_push16(REG1,DEFAULT);


	// STACK 16         STACK32
	// ------------------------------------
	// RA               REG45
	// filt_coeffs
	// out_buffer

	VoC_lw16i_short(REG4,SECTION_MSDF_SIZE,IN_PARALLEL);

	// ****************
	// input : REG0 : filter samples, INC0 = 2
	//         REG3 : source buffer,  INC3 = 2
	//         REG4 : nb_tap
	// output : none
	// used : REG02345, ACC01
	// ****************

	VoC_jal(CII_shift_filter);


	// ----------------------------------------
	// apply filter and store in the out buffer
	// ----------------------------------------


	// STACK 16         STACK32
	// ------------------------------------
	// filt_coeffs
	// out_buffer


	// filt_coeffs & out_buffer
	VoC_lw16_sd(REG2,1,DEFAULT);					// in RAM_Y
	VoC_pop16(REG3,DEFAULT);
			// out_buffer
	 VoC_lw16i_short(INC3,1,IN_PARALLEL);

	// STACK 16         STACK32
	// ------------------------------------
	// filt_coeffs



   VoC_lw16i_short(FORMAT32,3,DEFAULT);
	VoC_lw16i_set_inc(REG0,MIC_FILT_ADDR+1,1);      // in RAM_X
	VoC_lw16i_set_inc(REG1,MIC_FILT_ADDR+1,2);      // in RAM_X

	VoC_loop_i(1,160)

	    VoC_aligninc_pp(REG1,REG1,DEFAULT);
		VoC_inc_p(REG0,IN_PARALLEL);

		VoC_loop_i_short(SECTION_MSDF_SIZE,DEFAULT);
		VoC_lw32z(ACC0,IN_PARALLEL);
		VoC_startloop0
			VoC_bimac32inc_pp(REG1,REG2);
		VoC_endloop0

		VoC_lw16_sd(REG2,0,DEFAULT);				// in RAM_Y
		VoC_shr32_ri(ACC0,-5,DEFAULT);
		VoC_movreg16(REG1,REG0,DEFAULT);
exit_on_warnings = OFF;
		VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
exit_on_warnings = ON;

	VoC_endloop1

	VoC_pop16(REG0,DEFAULT);
	VoC_pop16(RA,DEFAULT);

	VoC_lw16i_short(FORMAT32,-1,DEFAULT);
	VoC_return;


}

/**************************************************************************************
 * Function:    Coolsand_AECRunDma
 *
 * Description: Run DMA 
 *
 * Inputs:      no
 *	
 *		REG2:	DMA local address;
 *		REG3:	DMA size;
 *		ACC0:	DMA extern address;
 *
 * Outputs:     no
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       08/13/2010
 **************************************************************************************/
void Rda_RunDma(void)
{
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
	VoC_sw16_d(REG3,CFG_DMA_SIZE);
	VoC_sw32_d(REG45,CFG_DMA_EADDR);

	VoC_cfg(CFG_CTRL_STALL);
	VoC_NOP();
	VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
	VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_PreProcessFiltering
 *
 * Description: pre-filter.
 *
 * Inputs:   REG0: input addr
 *			 REG2: output addr
 *
 * Outputs:  
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       09/09/2010
 **************************************************************************************/

/*
void Coolsand_Agc(void)
{

	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);

	VoC_loop_i(0,160)

		VoC_lw16_p(REG4,REG3,DEFAULT);

		//temp = ABS(in[index]);
		VoC_shr16_ri(REG4,15,DEFAULT);//sign = x >> (sizeof(int) * 8 - 1);
		VoC_movreg16(REG6,REG4,IN_PARALLEL);

		VoC_xor16_rr(REG6,REG4,DEFAULT);//x ^= sign;
		VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

		VoC_sub16_rr(REG6,REG6,REG4,DEFAULT); //x -= sign;
		VoC_lw32z(ACC0,IN_PARALLEL);

		//AGC_serror_l = MULTS(temp,AGC_Coeff_l) - AGC_MAXVAL;
		VoC_sub32_rd(ACC0,ACC0,GLOBAL_AGC_MAXVAL);
		
		VoC_lw32_d(REG45,GLOBAL_AGC_Coeff_l);

		VoC_mac32_rr(REG6,REG5,DEFAULT);
		VoC_shru16_ri(REG4,1,IN_PARALLEL);

		VoC_mac32_rr(REG6,REG4,DEFAULT);
		VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);
		
		VoC_lw32_d(ACC1,GLOBAL_AGC_acc_error_l);

		VoC_shr32_ri(ACC1,17,DEFAULT);

		//	if(AGC_serror_l <0)  AGC_serror_l =0;
		VoC_bnltz32_r(Speech_AGC_L1,ACC0)

		VoC_lw16i_short(ACC0_HI,-1,DEFAULT);
		VoC_lw16i_short(ACC0_LO,-1,IN_PARALLEL);
				
Speech_AGC_L1:
	//	AGC_acc_error_l = (AGC_acc_error_l >>17) + (AGC_serror_l <<1); 				
		VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
		VoC_lw16i_short(FORMAT32,14,IN_PARALLEL);

	//	AGC_Coeff_l = AGC_Coeff_l - AGC_acc_error_l ;	
		VoC_sub32_dr(REG45,GLOBAL_AGC_Coeff_l,ACC1);

		VoC_lw16i_short(RL7_LO,0,DEFAULT);
		VoC_lw16i_short(RL7_HI,2,IN_PARALLEL);

		VoC_bgtz32_r(Speech_AGC_L2,REG45)
			VoC_lw16i(REG4,0x7fff);
			VoC_lw16i_short(REG5,0,DEFAULT);
Speech_AGC_L2:

		VoC_bngt32_rr(Speech_AGC_L3,REG45,RL7)
			VoC_movreg32(REG45,RL7,DEFAULT);
Speech_AGC_L3:
	//	out[index] = MULTS(in[index] ,AGC_Coeff_l);
		VoC_shru16_ri(REG4,1,DEFAULT);
		VoC_movreg32(RL7,REG45,IN_PARALLEL);

		VoC_multf32_rp(ACC0,REG4,REG3,DEFAULT);

		VoC_mac32_rp(REG5,REG3,DEFAULT);
		VoC_lw16i_short(FORMAT32,-1,IN_PARALLEL);

		VoC_sw32_d(ACC1,GLOBAL_AGC_acc_error_l);

		VoC_shr32_ri(ACC0,-16,DEFAULT);

		VoC_sw32_d(RL7,GLOBAL_AGC_Coeff_l);

		VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);
	VoC_endloop0

// VoC_directive: PARSER_OFF
//		VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,1);
//		VoC_NOP();
//		VoC_NOP();
//		fwrite(&RAM_X[((REGS[3].reg==INPUT_SPEECH_BUFFER1_ADDR) ? INPUT_SPEECH_BUFFER1_ADDR : INPUT_SPEECH_BUFFER2_ADDR)/2],1,320,TestFileHandle);
// VoC_directive: PARSER_ON

	
	VoC_return;

}
*/

/**************************************************************************************
 * Function:    Coolsand_Aec
 *
 * Description: AEC.
 *
 * Inputs:   
 *			 
 *
 * Outputs:  
 *    
 * Used : ALL
 *
 * Version 1.0  Created by  Xuml       03/16/2011
 **************************************************************************************/

void Coolsand_Aec(void)
{

	VoC_push16(RA,DEFAULT);
/*
	VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);
	VoC_lw16d_set_inc(REG1,ENC_INPUT_ADDR_ADDR,2);

	VoC_lw16_d(REG2,AEC_MIC_SHIFT_GAIN);

	VoC_lw32inc_p(REG45,REG0,DEFAULT);
	

	VoC_loop_i(0,80)		
		VoC_shr16_rr(REG4,REG2,DEFAULT);
		VoC_shr16_rr(REG5,REG2,IN_PARALLEL);


		VoC_lw32inc_p(REG45,REG0,DEFAULT);		
		VoC_sw32inc_p(REG45,REG1,DEFAULT);		
	VoC_endloop0*/
	
	//backup speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AEC_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	
 	VoC_jal(Rda_RunDma);

	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AEC_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load AEC RAMX
	VoC_lw32_d(REG45,GLOBAL_AEC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AEC_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load AEC CODE
	VoC_lw32_d(REG45,GLOBAL_AEC_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AEC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);
	
	//High pass filter
	VoC_bez16_d(Coolsand_Encoder_HighPass,HIGH_PASS_FILTER_ENABLE_FLAG_ADDR)
		VoC_jal(Coolsand_HighPassFilter);
Coolsand_Encoder_HighPass:

	//AEC
	VoC_bez16_d(Coolsand_Encoder_Aec,AEC_ENABLE_FLAG_ADDR)
		VoC_jal(Coolsand_AEC);
Coolsand_Encoder_Aec:

//	VoC_bez16_d(Coolsand_Pitch_Filter,PITCH_FILTER_ENABLE_FLAG_ADDR)
//		VoC_jal(RDA_Pitch_Filter);
//Coolsand_Pitch_Filter:


	//backup AEC RAMX
	VoC_lw32_d(REG45,GLOBAL_AEC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AEC_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);


	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AEC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AEC_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma); 

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();


  	VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_Morph
 *
 * Description: no
 *
 * Inputs:       
 *	 
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

void Coolsand_Morph(void)
{
	VoC_push16(RA,DEFAULT);

	//backup speech RAMX
//	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
//    VoC_lw16i(REG2,MORPH_GLOBAL_X_BEGIN_ADDR/2);
//    VoC_lw16i(REG3,GLOBAL_MORPH_RAMX_BACKUP_SIZE);
//	VoC_or16_ri(REG5,0x4000);
//	
// 	VoC_jal(Rda_RunDma);


	//backup speech RAMY
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_MORPH_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	
 	VoC_jal(Rda_RunDma);


	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_MORPH_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load MORPH RAMX
	VoC_lw32_d(REG45,GLOBAL_AEC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_MORPH_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load MORPH CODE
	VoC_lw32_d(REG45,GLOBAL_MORPH_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_MORPH_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//AEC
	VoC_jal(CII_smbPitchShift);

	//backup MORPH RAMY
	VoC_lw32_d(REG45,GLOBAL_AEC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_MORPH_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_MORPH_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_MORPH_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma); 

//	//load speech RAMY
//	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMY_BACKUP_ADDR);
//    VoC_lw16i(REG2,MORPH_GLOBAL_Y_BEGIN_ADDR/2);
//    VoC_lw16i(REG3,GLOBAL_MORPH_RAMY_BACKUP_SIZE);
// 	VoC_jal(Rda_RunDma); 

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();

	VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_NoiseSuppresser
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
void Coolsand_NoiseSuppresser(void)
{
	VoC_lw16d_set_inc(REG3,ENC_INPUT_ADDR_ADDR,2);
	VoC_lw16d_set_inc(REG2,ENC_INPUT_ADDR_ADDR,2);

	VoC_lw16i_short(FORMAT16,(9-16),DEFAULT);//must be set with REG6 together
	VoC_lw16i(REG6,512);
	VoC_lw32inc_p(REG01,REG3,DEFAULT);
	VoC_movreg32(REG45,REG01,DEFAULT);

	VoC_loop_i(0,80)
		VoC_bgt16_rd(Coolsand_NoiseSuppress_L0,REG0,CONST_0_ADDR);
		VoC_sub16_dr(REG0,CONST_0_ADDR,REG0);
Coolsand_NoiseSuppress_L0:

		VoC_bgt16_rd(Coolsand_NoiseSuppress_L1,REG1,CONST_0_ADDR);
		VoC_sub16_dr(REG1,CONST_0_ADDR,REG1);
Coolsand_NoiseSuppress_L1:

		VoC_bgt16_rr(Coolsand_NoiseSuppress_L2,REG0,REG6)
		VoC_multf16_rr(REG4,REG0,REG4,DEFAULT);			
Coolsand_NoiseSuppress_L2:

		VoC_bgt16_rr(Coolsand_NoiseSuppress_L3,REG1,REG6)
		VoC_multf16_rr(REG5,REG1,REG5,DEFAULT);			
Coolsand_NoiseSuppress_L3:
		VoC_lw32inc_p(REG01,REG3,DEFAULT);
		VoC_movreg32(REG45,REG01,DEFAULT);
		VoC_sw32inc_p(REG45,REG2,DEFAULT);
	VoC_endloop0
			
	VoC_return;

}
*/
/**************************************************************************************
 * Function:    Coolsand_NotchFilter
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

void Coolsand_NotchFilter(void)
{

	VoC_lw16i_short(FORMAT32,0,DEFAULT);
	VoC_lw16i_short(FORMATX,-15,DEFAULT);

	VoC_lw16i(REG0,-16193);
	VoC_lw16i(REG1,-16002);

	VoC_lw16i_set_inc(REG2,GLOBAL_NOTCH_FILTER_XN12_YN12_ADDR,2);
	VoC_lw16i_set_inc(REG3,GLOBAL_NOTCH_FILTER_COEF_ADDR,1);

	VoC_loop_i(1,15)

		VoC_push32(REG23,DEFAULT);

		VoC_lw32inc_p(REG45,REG2,DEFAULT);
		VoC_lw32inc_p(RL6,REG2,DEFAULT);

		VoC_lw32_p(RL7,REG2,DEFAULT);
		VoC_shr32_ri(RL6,-1,IN_PARALLEL);

		VoC_lw16_p(REG2,REG3,DEFAULT);
		VoC_shr32_ri(RL7,-1,IN_PARALLEL);

		VoC_lw16_d(REG3,ENC_INPUT_ADDR_ADDR);
		
		VoC_loop_i(0,160)

			VoC_movreg32(REG67,RL6,DEFAULT);
			VoC_lw32z(ACC1,IN_PARALLEL);

			VoC_multf32_rr(ACC0,REG2,REG4,DEFAULT);//((int)31919*(xn_1))
			VoC_shru16_ri(REG6,1,IN_PARALLEL);
						
			VoC_mac32_rp(REG0,REG3,DEFAULT);//(int)-16193*x
			VoC_mac32_rr(REG2,REG6,IN_PARALLEL);//(int)31919*yn_1

			VoC_mac32_rr(REG0,REG5,DEFAULT);//(int)-16193*xn_2
			VoC_macX_rr(REG2,REG7,IN_PARALLEL);//(int)31919*yn_1

			VoC_movreg16(REG5,REG4,DEFAULT);// xn_2=xn_1;
			VoC_movreg32(REG67,RL7,IN_PARALLEL);
			
			VoC_shru16_ri(REG6,1,DEFAULT);
			VoC_macX_rr(REG1,REG7,IN_PARALLEL);//(int)-16002*yn_2

			VoC_lw16_p(REG4,REG3,DEFAULT);// xn_1=x;
			VoC_mac32_rr(REG1,REG6,IN_PARALLEL);//(int)-16002*yn_2

			VoC_shr32_ri(ACC0,14,DEFAULT);			
			VoC_shr32_ri(ACC1,14,DEFAULT);

			VoC_sub32_rr(ACC0,ACC1,ACC0,DEFAULT);
			VoC_movreg32(RL7,RL6,IN_PARALLEL);//  yn_2=yn_1;
						
			VoC_shr32_ri(ACC0,-16,DEFAULT);			
			VoC_movreg32(RL6,ACC0,IN_PARALLEL);//  yn_1=y;

			VoC_shr32_ri(RL6,-1,DEFAULT);

			VoC_sw16inc_p(ACC0_HI,REG3,DEFAULT);			

		VoC_endloop0

		VoC_pop32(REG23,DEFAULT);

		VoC_shr32_ri(RL6,1,DEFAULT);
		VoC_shr32_ri(RL7,1,IN_PARALLEL);

		VoC_sw32inc_p(REG45,REG2,DEFAULT);
		VoC_inc_p(REG3,IN_PARALLEL);

		VoC_sw32inc_p(RL6,REG2,DEFAULT);
		VoC_sw32inc_p(RL7,REG2,DEFAULT);

	VoC_endloop1

	VoC_return;

}

/**************************************************************************************
 * Function:    Rda_SpeexMicNoiseSuppresser
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
 * Version 1.0  Created by  Xuml       11/26/2013
 **************************************************************************************/
void Rda_SpeexMicNoiseSuppresser(void)
{
	VoC_push16(RA,DEFAULT);

	//backup speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load Mic Noise Suppress RAMX
	VoC_lw32_d(REG45,GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load  Noise Suppress CODE
	VoC_lw32_d(REG45,GLOBAL_NOISESUPPRESS_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//NOISE SUPPRESS
	VoC_lw16_d(REG0,GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR);
	VoC_lw16_d(REG1,ENC_INPUT_ADDR_ADDR);	
	VoC_lw16_d(REG2,DEC_OUTPUT_ADDR_ADDR);	
	//VoC_lw16_d(REG3,GLOBAL_MIC_NOISE_SUPPRESS_ECHO_STATE_ADDR);
	VoC_lw16i(REG3,0);

	//VoC_lw16_d(REG5,GLOBAL_MIC_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR);
	//VoC_lw16_d(REG4,GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR);
	VoC_lw32_d(REG45,GLOBAL_MIC_NOISE_SUPPRESS_SPEAKER_GAIN_ADDR);


	//VoC_sw16_d(REG0,GLABAL_NOISESUPPRESS_RESET_ADDR);
	//VoC_sw16_d(REG1,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR);
	VoC_sw32_d(REG01, GLABAL_NOISESUPPRESS_RESET_ADDR);
	VoC_sw16_d(REG2,GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR);
	VoC_sw16_d(REG3,GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR);
	//VoC_sw16_d(REG5,GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR);
	//VoC_sw16_d(REG4,GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);
	VoC_sw32_d(REG45, GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);

	// set params
	VoC_lw16_d(REG4,SPEEX_NOISE_SUPPRESS_DEFAULT_TX);
	VoC_lw32_d(REG01,SPEEX_NOISE_PROB_MIN_TX_RANGE1);
	VoC_lw32_d(REG23,SPEEX_NOISE_PROB_MIN_TX_RANGE3);

	VoC_sw16_d(REG4,SPEEX_NOISE_SUPPRESS_DEFAULT);
	VoC_sw32_d(REG01,SPEEX_NOISE_PROB_MIN_RANGE1);
	VoC_sw32_d(REG23,SPEEX_NOISE_PROB_MIN_RANGE3);

	VoC_jal(Rda_NoiseSuppress);

	VoC_lw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);
	VoC_NOP();
	VoC_sw16_d(REG1,GLOBAL_MIC_NOISE_SUPPRESS_RESET_ADDR);

	//backup Mic Noise Suppress RAMX
	VoC_lw32_d(REG45,GLOBAL_MIC_NOISESUPPRESS_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma); 

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();

	VoC_return;

}

/**************************************************************************************
 * Function:    Rda_SpeexReceiverNoiseSuppresser
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
 * Version 1.0  Created by  Xuml       11/26/2013
 **************************************************************************************/
void Rda_SpeexReceiverNoiseSuppresser(void)
{
	VoC_push16(RA,DEFAULT);

	//backup speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load Receiver Noise Suppress RAMX
	VoC_lw32_d(REG45,GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load Noise Suppress CODE
	VoC_lw32_d(REG45,GLOBAL_NOISESUPPRESS_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	
	VoC_lw16_d(REG0,GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR);
	VoC_lw16_d(REG1,DEC_OUTPUT_ADDR_ADDR);
	VoC_lw16_d(REG3,DEC_OUTPUT_ADDR_ADDR);	
	//VoC_lw16_d(REG2,GLOBAL_RECEIVER_NOISE_SUPPRESS_ECHO_STATE_ADDR);
	//VoC_lw16_d(REG4,GLOBAL_RECEIVER_NOISE_SUPPRESS_LEAK_ESTIMATE_ADDR);
	VoC_lw16i_short(REG2,0,DEFAULT);
	VoC_lw32z(REG45,IN_PARALLEL);

	//VoC_sw16_d(REG0,GLABAL_NOISESUPPRESS_RESET_ADDR);
	//VoC_sw16_d(REG1,GLABAL_NOISESUPPRESS_INPUTADDR_ADDR);
	VoC_sw32_d(REG01, GLABAL_NOISESUPPRESS_RESET_ADDR);
	//VoC_sw16_d(REG3,GLABAL_NOISESUPPRESS_SPEAKERADDR_ADDR);
	//VoC_sw16_d(REG2,GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR);
	VoC_sw32_d(REG23, GLOBAL_NOISESUPPRESS_ECHO_STATE_ADDR);
	//VoC_sw16_d(REG4,GLABAL_NOISESUPPRESS_LEAK_ESTIMATE_ADDR);
	//VoC_sw16_d(REG5,GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);
	VoC_sw32_d(REG45, GLABAL_NOISESUPPRESS_SPEAKER_GAIN_ADDR);

	// set params
	VoC_lw16_d(REG4,SPEEX_NOISE_SUPPRESS_DEFAULT_RX);
	VoC_lw32_d(REG01,SPEEX_NOISE_PROB_MIN_RX_RANGE1);
	VoC_lw32_d(REG23,SPEEX_NOISE_PROB_MIN_RX_RANGE3);

	VoC_sw16_d(REG4,SPEEX_NOISE_SUPPRESS_DEFAULT);
	VoC_sw32_d(REG01,SPEEX_NOISE_PROB_MIN_RANGE1);
	VoC_sw32_d(REG23,SPEEX_NOISE_PROB_MIN_RANGE3);
		
	//NOISE SUPPRESS
	VoC_jal(Rda_NoiseSuppress);

	VoC_lw16_d(REG1,GLABAL_NOISESUPPRESS_RESET_ADDR);
	VoC_NOP();
	VoC_sw16_d(REG1,GLOBAL_RECEIVER_NOISE_SUPPRESS_RESET_ADDR);

	//backup Receiver Noise Suppress RAMX
	VoC_lw32_d(REG45,GLOBAL_RECEIVER_NOISESUPPRESS_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_NOISESUPPRESS_TOTAL_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma); 

	VoC_pop16(RA,DEFAULT);
	VoC_NOP();

	VoC_return;
}

void Rda_MicAgc(void)
{
	VoC_push16(RA, DEFAULT);

	//backup speex RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_TOTAL_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load Mic AGC RAMX
	VoC_lw32_d(REG45,GLOBAL_MIC_AGC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load AGC CODE
	VoC_lw32_d(REG45,GLOBAL_AGC_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load AGC CONST
	VoC_lw32_d(REG45,AGC1_ConstY_ExternalAddr_addr);
    VoC_lw16i(REG2,kGenFuncTable_addr/2);
    VoC_lw16i(REG3,SECTION_AGC_CONSTX_SIZE);
	VoC_jal(Rda_RunDma);

	// nonclip
	VoC_bez16_d(Rda_MicAgc_Nonclip,MIC_NONCLIP_ENABLE_FLAG_ADDR)
		//VoC_lw16_d(REG3,GLOBAL_MIC_DIGITAL_GAIN_ADDR);
		//VoC_lw16_d(REG4,GLOBAL_MIC_DIGITAL_MAXVALUE_ADDR);
		VoC_lw32_d(RL6,GLOBAL_MIC_DIGITAL_GAIN_ADDR);
		VoC_lw16_d(REG5,GLOBAL_MIC_DIGITAL_GAIN_ADDR);
		
		VoC_lw16d_set_inc(REG0,ENC_INPUT_ADDR_ADDR,2);
		VoC_lw16i_set_inc(REG1,GLOBAL_NON_CLIP_TEMP_BUFFER,2);
		VoC_lw16i_set_inc(REG2,GLOBAL_NON_CLIP_MIC_HISTORYDATA,2);	
		//VoC_sw16_d(REG3,GLOBAL_DIGITAL_GAIN_ADDR);
		//VoC_sw16_d(REG4,GLOBAL_DIGITAL_MAXVALUE_ADDR);
		VoC_sw32_d(RL6,GLOBAL_DIGITAL_GAIN_ADDR);
		VoC_sw16_d(REG5,GLOBAL_NOISE_DIGITAL_GAIN_ADDR);

		VoC_lw32_d(ACC0,GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_TX);
		VoC_lw16i(REG3,GLOBAL_MIC_DIGITAL_OLD_GAIN_ADDR);
		VoC_sw32_d(ACC0,GLOBAL_NON_CLIP_SMOOTH_THRESHOLD);

		VoC_jal(Coolsand_NonClipDigitalGain);
Rda_MicAgc_Nonclip:

	// digital agc from webrtc
	VoC_bez16_d(Rda_MicAgc_WebRtcAgc,STRUCT_MicAgcConfig_enable_addr)
		VoC_bez16_d(Rda_MicAgc_SKIP_CONFIG, GLOBAL_MICAGC_RELOAD)
			////initial AgcConfig struct////
			VoC_lw32_d(ACC0,STRUCT_MicAgcConfig_enable_addr);
			VoC_lw32_d(ACC1,STRUCT_MicAgcConfig_compressionGaindB_addr);
			VoC_lw32_d(RL6,STRUCT_MicAgcConfig_upperThr_addr);
			VoC_lw32_d(RL7,STRUCT_MicAgcConfig_decayValue_addr);
			VoC_lw32_d(REG01,STRUCT_MicAgcConfig_capacitorFastFactor_addr);
			VoC_lw32_d(REG23,STRUCT_MicAgcConfig_NoiseGateThr_addr);
			VoC_sw32_d(ACC0,STRUCT_AgcConfig_enable_addr);
			VoC_sw32_d(ACC1,STRUCT_AgcConfig_compressionGaindB_addr);
			VoC_sw32_d(RL6,STRUCT_AgcConfig_upper_thr_addr);
			VoC_sw32_d(RL7,STRUCT_AgcConfig_decay_value_addr);
			VoC_sw32_d(REG01,STRUCT_AgcConfig_capacitorFastFactor_addr);
			VoC_sw32_d(REG23,STRUCT_AgcConfig_NoiseGateThr_addr);

			VoC_jal(Agc1_Main);

			VoC_lw16i_short(REG0, 0, DEFAULT);
			VoC_NOP();
			VoC_sw16_d(REG0, GLOBAL_MICAGC_RELOAD);
Rda_MicAgc_SKIP_CONFIG:

		VoC_lw16_d(REG0, ENC_INPUT_ADDR_ADDR);
		VoC_NOP();
		VoC_sw16_d(REG0, buf_addr_addr);

		VoC_jal(Rda_ProcessWebRtcAgc);
Rda_MicAgc_WebRtcAgc:

	//backup Mic AGC RAMX
	VoC_lw32_d(REG45,GLOBAL_MIC_AGC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);


	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	
	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_TOTAL_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	VoC_pop16(RA, DEFAULT);
	VoC_NOP();
	VoC_return;
}

void Rda_ReceiverAgc(void)
{
	VoC_push16(RA, DEFAULT);

	//backup speex RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_TOTAL_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

	//backup speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);

  	//load Receiver AGC RAMX
	VoC_lw32_d(REG45,GLOBAL_RECEIVER_AGC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load AGC CODE
	VoC_lw32_d(REG45,GLOBAL_AGC_CODE_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	//load AGC CONST
	VoC_lw32_d(REG45,AGC1_ConstY_ExternalAddr_addr);
    VoC_lw16i(REG2,kGenFuncTable_addr/2);
    VoC_lw16i(REG3,SECTION_AGC_CONSTX_SIZE);
	VoC_jal(Rda_RunDma);

	// process nonclip
	VoC_bez16_d(Rda_ReceiverAgc_NonClip,RECEIVER_NONCLIP_ENABLE_FLAG_ADDR)
		VoC_lw16i_short(REG5,4,DEFAULT);
		//VoC_lw16_d(REG3,GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR);
		//VoC_lw16_d(REG4,GLOBAL_RECEIVER_DIGITAL_MAXVALUE_ADDR);
		VoC_lw32_d(RL6, GLOBAL_RECEIVER_DIGITAL_GAIN_ADDR);
		VoC_lw16d_set_inc(REG0,DEC_OUTPUT_ADDR_ADDR,2);
		VoC_lw16i_set_inc(REG1,GLOBAL_NON_CLIP_TEMP_BUFFER,2);
		VoC_lw16i_set_inc(REG2,GLOBAL_NON_CLIP_SPK_HISTORYDATA,2);		
		//VoC_sw16_d(REG3,GLOBAL_DIGITAL_GAIN_ADDR);
		//VoC_sw16_d(REG4,GLOBAL_DIGITAL_MAXVALUE_ADDR);
		VoC_sw32_d(RL6,GLOBAL_DIGITAL_GAIN_ADDR);
		VoC_sw16_d(REG5,GLOBAL_NOISE_DIGITAL_GAIN_ADDR);

		VoC_lw32_d(ACC0,GLOBAL_NON_CLIP_SMOOTH_THRESHOLD_RX);
		VoC_lw16i(REG3,GLOBAL_RECEIVER_DIGITAL_OLD_GAIN_ADDR);
		VoC_sw32_d(ACC0, GLOBAL_NON_CLIP_SMOOTH_THRESHOLD);
		VoC_jal(Coolsand_NonClipDigitalGain);
Rda_ReceiverAgc_NonClip:

	// process webrtc digital agc
	VoC_bez16_d(Rda_ReceiverAgc_WebrtcAgc,STRUCT_ReceiverAgcConfig_enable_addr)
		VoC_bez16_d(Rda_ReceiverAgc_SKIP_CONFIG, GLOBAL_RECEIVERAGC_RELOAD)
			////initial AgcConfig struct////
			VoC_lw32_d(ACC0,STRUCT_ReceiverAgcConfig_enable_addr);
			VoC_lw32_d(ACC1,STRUCT_ReceiverAgcConfig_compressionGaindB_addr);
			VoC_lw32_d(RL6,STRUCT_ReceiverAgcConfig_upperThr_addr);
			VoC_lw32_d(RL7,STRUCT_ReceiverAgcConfig_decayValue_addr);
			VoC_lw32_d(REG01,STRUCT_ReceiverAgcConfig_capacitorFastFactor_addr);
			VoC_lw32_d(REG23,STRUCT_ReceiverAgcConfig_NoiseGateThr_addr);
			VoC_sw32_d(ACC0,STRUCT_AgcConfig_enable_addr);
			VoC_sw32_d(ACC1,STRUCT_AgcConfig_compressionGaindB_addr);
			VoC_sw32_d(RL6,STRUCT_AgcConfig_upper_thr_addr);
			VoC_sw32_d(RL7,STRUCT_AgcConfig_decay_value_addr);
			VoC_sw32_d(REG01,STRUCT_AgcConfig_capacitorFastFactor_addr);
			VoC_sw32_d(REG23,STRUCT_AgcConfig_NoiseGateThr_addr);

			VoC_jal(Agc1_Main);

			VoC_lw16i_short(REG0, 0, DEFAULT);
			VoC_NOP();
			VoC_sw16_d(REG0, GLOBAL_RECEIVERAGC_RELOAD);
Rda_ReceiverAgc_SKIP_CONFIG:

		VoC_lw16_d(REG0, DEC_OUTPUT_ADDR_ADDR);
		VoC_NOP();
		VoC_sw16_d(REG0, buf_addr_addr);

		VoC_jal(Rda_ProcessWebRtcAgc);
Rda_ReceiverAgc_WebrtcAgc:

	// process iir eq
	VoC_bez16_d(Rda_ReceiverAgc_EQ,IIR_ENABLE_FLAG_ADDR)
		VoC_jal(Rda_ProcessIIR);
Rda_ReceiverAgc_EQ:

	//backup RECEIVER AGC RAMX
	VoC_lw32_d(REG45,GLOBAL_RECEIVER_AGC_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_RAMX_BACKUP_SIZE);
	VoC_or16_ri(REG5,0x4000);
	VoC_jal(Rda_RunDma);


	//load speech code
	VoC_lw32_d(REG45,GLOBAL_SPEECH_CODE_BACKUP_ADDR);
    VoC_lw16i(REG2,SECTION_1_START);
    VoC_lw16i(REG3,GLOBAL_AGC_CODE_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	
	//load speech RAMX
	VoC_lw32_d(REG45,GLOBAL_SPEECH_RAMX_BACKUP_ADDR);
    VoC_lw16i(REG2,AMR_EFR_RAMX_CONSTANTS_ADDR/2);
    VoC_lw16i(REG3,GLOBAL_AGC_TOTAL_RAMX_BACKUP_SIZE);
	VoC_jal(Rda_RunDma);

	VoC_pop16(RA, DEFAULT);
	VoC_NOP();
	VoC_return;
}

/**************************************************************************************
 * Function:    Rda_ProcessMic
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
 * Version 1.0  Created by  Xuml       01/02/2014
 **************************************************************************************/
void Rda_ProcessMic(void)
{
	VoC_push16(RA,DEFAULT);

	VoC_lw16_d(REG2,ENC_INPUT_ADDR_ADDR);

	VoC_lw16i(REG0,INPUT_SPEECH_BUFFER2_ADDR);

	VoC_lw32_d(REG45,GLOBAL_TX_ADDR);

	VoC_be16_rr(Rda_ProcessMic_L1,REG0,REG2)

		VoC_lw16i(ACC0_HI,320);
		VoC_shr32_ri(ACC0,16,DEFAULT);
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

Rda_ProcessMic_L1:
		VoC_or16_ri(REG5,0x4000);

		VoC_shr16_ri(REG2,1,DEFAULT);
		VoC_lw16i(REG3,80);
		VoC_jal(Rda_RunDma);


	VoC_lw16i_short(REG5,1,DEFAULT);
	VoC_NOP();
	VoC_sw16_d(REG5,GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG);
	VoC_NOP();

	VoC_cfg(CFG_CTRL_IRQ_TO_BCPU|CFG_CTRL_STALL);

	// VoC_directive: PARSER_OFF
	VoC_cfg_stop;
	// VoC_directive: PARSER_ON

	VoC_NOP();
	VoC_NOP();

	VoC_lw16_d(REG7,CFG_WAKEUP_STATUS);

	VoC_lw16i_short(REG5,0,DEFAULT);
		// clear the wakeup status
	VoC_sw16_d(REG7,CFG_WAKEUP_STATUS);

	VoC_sw16_d(REG5,GLOBAL_EXTERN_PROCESS_MIC_HANDLE_FLAG);


	VoC_lw16_d(REG2,ENC_INPUT_ADDR_ADDR);

	VoC_lw16i(REG0,INPUT_SPEECH_BUFFER2_ADDR);

	VoC_lw32_d(REG45,GLOBAL_TX_ADDR);

	VoC_be16_rr(Rda_ProcessMic_L2,REG0,REG2)

		VoC_lw16i(ACC0_HI,320);
		VoC_shr32_ri(ACC0,16,DEFAULT);
		VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);

Rda_ProcessMic_L2:			
		VoC_shr16_ri(REG2,1,DEFAULT);
		VoC_lw16i(REG3,80);
		VoC_jal(Rda_RunDma);


	VoC_pop16(RA,DEFAULT);
	VoC_NOP();
	VoC_return;

}
