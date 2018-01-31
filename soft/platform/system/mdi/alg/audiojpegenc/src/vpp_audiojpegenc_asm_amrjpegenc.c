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

#include "voc2_library.h"


#include "vppp_audiojpegenc_common.h"


// VoC_directive: voc_stack16
#define SP_16_ADDR                                      RAM_X_MAX - 1 // measured minimum : 0x1000 - 0x27
// VoC_directive: voc_stack32
#define SP_32_ADDR                                      RAM_Y_MAX - 2 // measured minimum : 0x1800 - 0x19


#if 0


voc_short  GLOBAL_RESET_FLAG_ADDR ,y
voc_short  GLOBAL_RELOAD_RESET_FLAG_ADDR ,y

voc_short  AMR_GLOBAL_VARS_Y_START_ADDR ,40,y
voc_short  STRUCT_COD_AMRSTATE_H1_ADDR  ,40,y
voc_alias  AMR_LOCAL_VARS_Y_LEVEL0_START_ADDR   (40+STRUCT_COD_AMRSTATE_H1_ADDR),y
voc_short  COD_AMR_H1_SF0_ADDRESS ,40,y
voc_short  COD_AMR_ANA_ADDR   ,60,y

voc_struct AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR,y
voc_short C1035PF_DN_ADDR[40]
voc_short C1035PF_RR_DIAG_ADDR[40]
voc_short C1035PF_RR_SIDE_ADDR[780]
voc_short SEARCH_CODE_RRV_COE_ADDR
voc_short SEARCH_CODE_RR_COE_ADDR
voc_short SEARCH_10I40_I_ADDR[2]
voc_short SEARCH_10I40_PSK_ADDR
voc_short SEARCH_10I40_ALPK_ADDR
voc_short C1035PF_RR_DIAG_ADDR_2_ADDR
voc_short SEARCH_CODE_CP_RR_ADDR_ADDR
voc_short SEARCH_CODE_DN_ADDR_ADDR
voc_short SEARCH_CODE_RRV_ADDR_ADDR
voc_short C1035PF_RR_SIDE_ADDR_ADDR
voc_short C1035PF_RR_DIAG_ADDR_ADDR
voc_short SEARCH_CODE_CP_RR_7_ADDR_ADDR
voc_short RR_SIDE_DIAG_TEMP_ADDR
voc_short SEARCH_10I40_I2_TEMP_ADDR
voc_short SEARCH_10I40_I2I3_TEMP_ADDR
voc_short SEARCH_CODE_DN_RRV_ADDR_ADDR[2]
voc_short SEARCH_CODE_ALP0_TEMP_ADDR[2]
voc_short SEARCH_10I40_I0_ADDR
voc_short SEARCH_10I40_I1_ADDR
voc_short SEARCH_10I40_I2_ADDR
voc_short SEARCH_10I40_I3_ADDR
voc_short SEARCH_10I40_I4_ADDR
voc_short SEARCH_10I40_I5_ADDR
voc_short SEARCH_10I40_I6_ADDR
voc_short SEARCH_10I40_I7_ADDR
voc_short SEARCH_10I40_I8_ADDR
voc_short SEARCH_10I40_I9_ADDR
voc_short linear_signs_ADDR[4]
voc_short linear_codewords_ADDR[8]
voc_struct_end

voc_alias Pitch_ol_M122                           (0+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR),y
voc_alias PITCH_OL_M122_SCALED_SIGNAL_ADDR        (0 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_P_MAX1_ADDR               (310 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_MAX1_ADDR                 (311 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_P_MAX2_ADDR               (312 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_MAX2_ADDR                 (313 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_P_MAX3_ADDR               (314 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_MAX3_ADDR                 (315 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_SCAL_FAC_ADDR             (316 + Pitch_ol_M122) ,y
voc_alias PITCH_OL_M122_CORR_ADDR                 (322 + Pitch_ol_M122) ,y

voc_alias EFR_GLOBAL_AMR_LOCAL                    (80+AMR_GLOBAL_VARS_Y_START_ADDR) ,y
voc_alias COD_AMR_CODE_SF0_ADDRESS                (0 + EFR_GLOBAL_AMR_LOCAL)    ,y
voc_alias COD_AMR_XN_SF0_ADDRESS                  (42 + EFR_GLOBAL_AMR_LOCAL)   ,y
voc_alias COD_AMR_Y2_SF0_ADDRESS                  (82 + EFR_GLOBAL_AMR_LOCAL)   ,y
voc_alias COD_AMR_T0_SF0_ADDRESS                  (122 + EFR_GLOBAL_AMR_LOCAL)  ,y
voc_alias COD_AMR_T0_FRAC_SF0_ADDRESS             (123 + EFR_GLOBAL_AMR_LOCAL)  ,y
voc_alias COD_AMR_I_SUBFR_SF0_ADDRESS             (124 + EFR_GLOBAL_AMR_LOCAL)  ,y
voc_alias COD_AMR_GAIN_PIT_SF0_ADDRESS            (126 + EFR_GLOBAL_AMR_LOCAL)  ,y
voc_alias COD_AMR_GAIN_CODE_SF0_ADDRESS           (127 + EFR_GLOBAL_AMR_LOCAL)  ,y

voc_alias CONST32_ADDR                          (linear_codewords_ADDR +8),y
voc_short CONST_D_1_4_ADDR     ,2,y
voc_short CONST_D_1_8_ADDR     ,2,y
voc_short CONST_D_1_16_ADDR    ,2,y
voc_short CONST_D_1_32_ADDR    ,2,y
voc_short CONST_D_1_64_ADDR    ,2,y
////////////////////const 32 end//////////////////////////////////////964 short
voc_short JPEGENC_MYDCT_ADDR[12],y
voc_short CONST_MYQUN_TAB0_ADDR[128] ,y
voc_short CONST_MYQUN_TAB1_ADDR[128] ,y
voc_short CONST_MYHUFF_DC0_ADDR[24]  ,y
voc_short CONST_MYHUFF_DC1_ADDR[24]  ,y
voc_short CONST_MYHUFF_AC0_ADDR[324] ,y
voc_short CONST_MYHUFF_AC1_ADDR[324] ,y
////////////////////////////////////////////////////////////////////////4994  short
voc_short TABLE_WINDOW_160_80_ADDR[240] ,y
voc_short TABLE_WINDOW_232_8_ADDR[240] ,y
voc_short TABLE_MEAN_LSF_ADDR[10] ,y
voc_short TABLE_SLOPE_ADDR[64] ,y
voc_short TABLE_LSP_LSF_ADDR[66] ,y
voc_short TABLE_LOG2_ADDR[34] ,y
voc_short TABLE_LAG_L_ADDR[10] ,y
voc_short TABLE_LAG_H_ADDR[10] ,y
voc_short TABLE_INV_SQRT_ADDR[50] ,y
voc_short TABLE_GRID_ADDR[62] ,y
voc_short TABLE_POW2_ADDR[34] ,y
voc_short STATIC_CONST_QUA_GAIN_PITCH_ADDR[16] ,y
voc_short STATIC_CONST_INTER_6_ADDR[62] ,y
voc_short STATIC_CONST_INTER_6_25_ADDR[26] ,y
voc_short STATIC_CONST_F_GAMMA2_ADDR[10] ,y
voc_short STATIC_CONST_F_GAMMA1_ADDR[10] ,y
voc_short STATIC_CONST_GRAY_ADDR[8] ,y
voc_short TABLE_DICO1_LSF_ADDR[512] ,y
voc_short TABLE_DICO2_LSF_ADDR[1024] ,y
voc_short TABLE_DICO3_LSF_ADDR[1024] ,y
voc_short TABLE_DICO4_LSF_ADDR[1024] ,y
voc_short TABLE_DICO5_LSF_ADDR[256] ,y
voc_short STATIC_CONST_BITNO_ADDR[30] ,y
voc_short STATIC_CONST_qua_gain_code_ADDR[96] ,y
voc_short STATIC_CONST_gamma1_ADDR[10] ,y
voc_short STATIC_CONST_pred_MR122_ADDR[4] ,y
voc_short AMR_ENC_RESET_TABLE_ADDR[202] ,y
voc_short MATRIC_RGB_ADDR[8] ,y
//voc_short TEMP_BUFF_64_ADDR[32] ,y
voc_short GLOBAL_OUT_FOR1280BY1024_ADDR[48],y
voc_short GLOBAL_OUT_48_1_ADDR[48],y
voc_short GLOBAL_OUT_48_2_ADDR[48],y
voc_short GLOBAL_OUT_48_3_ADDR[48],y
voc_short GLOBAL_OUT_48_4_ADDR[48],y
voc_short GLOBAL_OUT_48_5_ADDR[48],y
voc_short GLOBAL_OUT_48_6_ADDR[48],y
voc_short GLOBAL_OUT_48_7_ADDR[48],y
voc_alias GLOBAL_TEMP_BUF_ADDR   GLOBAL_OUT_FOR1280BY1024_ADDR,y
voc_alias GLOBAL_TEMP_BUF2_ADDR  GLOBAL_TEMP_BUF_ADDR+352,     y

voc_word GLOBAL_xScale __attribute__((export)) ,y
voc_word GLOBAL_yScale __attribute__((export)) ,y

voc_word GLOBAL_input_p_Y_Row,y
voc_word GLOBAL_input_p_Y_Colume,y
voc_word GLOBAL_input_p_UV_Row,y
voc_word GLOBAL_input_p_UV_Colume,y

voc_short GLOBAL_McuZommInRow,y
voc_short GLOBAL_McuZoomINColume,y


voc_short GLOBAL_DmaInputRows,y
voc_short GLOBAL_DmaInputColumes,y


voc_short GLOBAL_InputRowsOffset,y
voc_short GLOBAL_McuInputBuffAddr,y

voc_word GLOBAL_DmaInputBufferDddr,y


voc_short GLOBAL_RGBToYUV422Coef[12],y






//voc_short GLOBAL_TEMP_FOR_VIDEO[704-384],Y

voc_alias GLOBAL_OUT_32_1_ADDR    GLOBAL_OUT_FOR1280BY1024_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_2_ADDR    GLOBAL_OUT_32_1_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_3_ADDR    GLOBAL_OUT_32_2_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_4_ADDR    GLOBAL_OUT_32_3_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_5_ADDR    GLOBAL_OUT_32_4_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_6_ADDR    GLOBAL_OUT_32_5_ADDR+32 ,y
voc_alias GLOBAL_OUT_32_7_ADDR    GLOBAL_OUT_32_6_ADDR+32 ,y

//global vars in ram_x
voc_short JPEGENC_OUT_ADDR[744],x
voc_alias OUTPUT_BITS_ADDR                JPEGENC_OUT_ADDR ,x
voc_alias INPUT_SPEECH_BUFFER_ADDR       (16 + JPEGENC_OUT_ADDR),x

//voc_short ALL_GLOBAL_VARS_X_START_ADDR  ,64,x
//voc_short GSM_GLOBAL_VARS_X_START_ADDR  ,64,x
//voc_short AMR_GLOBAL_VARS_X_START_ADDR  ,236,x

//
voc_short TEMP_MYRING_ADDR[64+64+236],x  //,248,x
//
voc_short STATIC_L_SACF_ADDR ,56,x
voc_short SCAL_ACF_ADDR_P   ,4,x
voc_short PRAM4_TABLE_ADDR  ,10,x

voc_struct  STRUCT_PRE_PROCESSSTATE,x
voc_short STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR
voc_short STRUCT_PRE_PROCESSSTATE_Y2_HI_ADDR
voc_short STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR
voc_short STRUCT_PRE_PROCESSSTATE_Y1_HI_ADDR
voc_short STRUCT_PRE_PROCESSSTATE_X0_ADDR
voc_short STRUCT_PRE_PROCESSSTATE_X1_ADDR
voc_struct_end

//STRUCT_COD_AMRSTATE
voc_struct  STRUCT_COD_AMRSTATE ,x
voc_short STRUCT_COD_AMRSTATE_ADDR[40]
voc_short STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR[40]
voc_short STRUCT_COD_AMRSTATE_P_WINDOW_ADDR[40]
voc_short STRUCT_COD_AMRSTATE_SPEECH_ADDR[40]
voc_short STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR[161]
voc_short STRUCT_COD_AMRSTATE_OLD_WSP_ADDR[143]
voc_short STRUCT_COD_AMRSTATE_WSP_ADDR[160]
voc_short STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR[6]
voc_short STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR[2]
voc_short STRUCT_COD_AMRSTATE_OLD_EXC_ADDR[154]
voc_short STRUCT_COD_AMRSTATE_EXC_ADDR[161]
voc_short STRUCT_COD_AMRSTATE_AI_ZERO_ADDR[11]
voc_short STRUCT_COD_AMRSTATE_ZERO_ADDR[41]
voc_struct_end

voc_short STRUCT_LEVINSONSTATE_ADDR[11],x
voc_short STRUCT_LSPSTATE_ADDR[10],x
voc_short STRUCT_LSPSTATE_LSP_OLD_Q_ADDR[10],x
voc_short STRUCT_Q_PLSFSTATE_ADDR[10],x
voc_short STRUCT_CLLTPSTATE_ADDR[18],x

//STRUCT_GC
voc_short STRUCT_GC_PREDST_ADDR[30],x


//STRUCT_TONSTABSTATE
voc_struct STRUCT_TONSTABSTATE,x
voc_short STRUCT_TONSTABSTATE_ADDR[7]
voc_short STRUCT_TONSTABSTATE_COUNT_ADDR
voc_struct_end

voc_short   STRUCT_VADSTATE1[70], x

//STRUCT_DTX_ENCSTATE
voc_struct  STRUCT_DTX_ENCSTATE ,x
voc_short STRUCT_DTX_ENCSTATE_ADDR[80]
voc_short STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR[8]
voc_short STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR
voc_short STRUCT_DTX_ENCSTATE_LOG_EN_INDEX_ADDR
voc_short STRUCT_DTX_ENCSTATE_INIT_LSF_VQ_INDEX_ADDR[2]
voc_short STRUCT_DTX_ENCSTATE_LSP_INDEX_ADDR[4]
voc_short STRUCT_DTX_ENCSTATE_DTXHANGOVERCOUNT_ADDR
voc_short STRUCT_DTX_ENCSTATE_DECANAELAPSEDCOUNT_ADDR
voc_struct_end

voc_short STRUCT_COD_AMRSTATE_MEM_SYN_ADDR[10] ,x
voc_short STRUCT_COD_AMRSTATE_MEM_W0_ADDR[10] ,x
voc_short STRUCT_COD_AMRSTATE_MEM_W_ADDR[10] ,x
voc_short STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR[10] ,x
voc_short STRUCT_COD_AMRSTATE_ERROR_ADDR[40] ,x
voc_short STRUCT_COD_AMRSTATE_SHARP_ADDR,2 ,x

voc_struct AMR_LOCAL_VARS_X_LEVEL0_START_ADDR,x
voc_short BEGIN_OF_COD_AMR[40]
voc_short COD_AMR_XN2_ADDRESS[40]
voc_short COD_AMR_Y1_ADDRESS[40]
voc_short COD_AMR_Y2_ADDRESS[40]
voc_short COD_AMR_RES_ADDRESS[40]
voc_short COD_AMR_MEM_SYN_SAVE_ADDRESS[10]
voc_short COD_AMR_MEM_W0_SAVE_ADDRESS[10]
voc_short COD_AMR_MEM_ERR_SAVE_ADDRESS[10]
voc_short COD_AMR_GCOEFF_ADDRESS[4]
voc_short COD_AMR_SUBFRNR_ADDRESS
voc_short COD_AMR_I_SUBFR_ADDRESS
voc_short COD_AMR_T_OP_ADDRESS[2]
voc_short COD_AMR_T0_ADDRESS
voc_short COD_AMR_T0_FRAC_ADDRESS
voc_short COD_AMR_GAIN_PIT_ADDRESS
voc_short COD_AMR_GAIN_CODE_ADDRESS
voc_short COD_AMR_EXC_ADDR_ADDRESS[2]
voc_short COD_AMR_LSP_FLAG_ADDRESS
voc_short COD_AMR_GP_LIMIT_ADDRESS
voc_short COD_AMR_VAD_FLAG_ADDRESS
voc_short COD_AMR_COMPUTE_SID_FLAG_ADDRESS
voc_short COD_AMR_A_T_ADDRESS[44]
voc_short COD_AMR_AQ_T_ADDRESS[44]
voc_short COD_AMR_LSP_NEW_ADDRESS[10]
voc_short COD_AMR_SHARP_SAVE_ADDRESS
voc_short COD_AMR_EVENSUBFR_ADDRESS
voc_short COD_AMR_CODE_ADDRESS[40]
voc_struct_end

//LEVEL 1
voc_alias AMR_LOCAL_VARS_X_LEVEL1_START_ADDR      (388+AMR_LOCAL_VARS_X_LEVEL0_START_ADDR),x

//ALIAS
voc_struct SET_SIGN_X    ,x
voc_short SEARCH_10I40_RRV_ADDR[40]
voc_short SEARCH_CODE_CP_RR_ADDR[320]
voc_short SEARCH_CODE_PS0_TEMP_ADDR
voc_short SEARCH_10I40_PS0_ADDR
voc_short C1035PF_IPOS_ADDR[10]
voc_short C1035PF_POS_MAX_ADDR[10]
voc_short C1035PF_CODVEC_ADDR[10]
voc_short C1035PF_SIGN_ADDR[40]
voc_struct_end

//ALIAS
voc_alias AZ_LSP_IP_ADDR          (0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR),x

//ALIAS
voc_alias Q_plsf_5_X                 (0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR),x
voc_alias Q_PLSF_5_LSF1_ADDR         (0 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_LSF2_ADDR         (10 + Q_plsf_5_X),x
voc_alias Q_PLSF_5_WF1_ADDR          (20 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_WF2_ADDR          (30 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_LSF_R1_ADDR       (40 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_LSF_R2_ADDR       (50 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_LSF1_Q_ADDR       (60 + Q_plsf_5_X) ,x
voc_alias Q_PLSF_5_LSF2_Q_ADDR       (70 + Q_plsf_5_X) ,x

///////////////const data////////////////////////////////////////////
voc_short CONST_inverse_Zig_Zag_ADDR,64,x
voc_short CONST_128_ADDR,x
voc_short CONST_256_ADDR ,x
voc_short CONST_neg3_ADDR ,x
voc_short CONST_143_ADDR  ,x
voc_short CONST_160_ADDR  ,x
voc_short CONST_26214_ADDR ,x
voc_short CONST_205_ADDR  ,x
voc_short CONST_0x199a_ADDR ,x
voc_short CONST_640_INT_ADDR,x
voc_short CONST_640BY8_INT_ADDR,x
voc_short CONST_640BY9_INT_ADDR,x
voc_short CONST_479_ADDR,x
voc_short CONST_640_ADDR[2],x
voc_short CONST_0X80_ADDR[2],x

voc_short CONST_0x00010001_ADDR  ,2,x
voc_short CONST_0x00020002_ADDR  ,2,x
voc_short CONST_0x0000FFFF_ADDR  ,2,x
voc_alias CONST_0xFFFF_ADDR        CONST_0x0000FFFF_ADDR ,x
voc_alias CONST_0x0000ffffL_ADDR   CONST_0x0000FFFF_ADDR ,x
voc_short CONST_0x00080008_ADDR   ,2,x
voc_short CONST_0x40000000_ADDR   ,2,x
voc_short CONST_0x7FFFFFFF_ADDR   ,2,x
voc_short CONST_0x80000000_ADDR   ,2,x
voc_short CONST_0x8000FFFF_ADDR   ,2,x
voc_short CONST_TAB244_ADDR       ,244,x
//64+24+244 = 332 short
voc_short CONST_BLOCKSIZE_BYTE_ADDR      ,2,x
voc_short CONST_WIDTH_ADD8    ,x
voc_short CONST_WIDTH_SUB8    ,x
voc_short CONST_WIDTH_MPY8    ,x
voc_short CONST_WIDTH_MPY8_ADD8  ,x

voc_short JPEGENC_BUFF_Y_ADDR[64]   ,  x
voc_short JPEGENC_BUFF_Y1_ADDR[64]  ,  x
voc_short JPEGENC_BUFF_Y2_ADDR[64]  ,  x
voc_short JPEGENC_BUFF_Y3_ADDR[64]  ,  x
voc_short JPEGENC_BUFF_CB_ADDR[64]  ,  x
voc_short JPEGENC_BUFF_CR_ADDR[64]  ,  x

voc_short GLOBAL_4WIDTH_ADDR      ,  x
voc_short GLOBAL_OLD_DCY_ADDR     ,  x
voc_short GLOBAL_OLD_DCU_ADDR     ,  x
voc_short GLOBAL_OLD_DCV_ADDR     ,  x
voc_short GLOBAL_SHORT_1_ADDR     ,  x
voc_short GLOBAL_CURR_POS_ADDR    ,  x
voc_short GLOBAL_CURRBITS_ADDR    ,  x
voc_short GLOBAL_CURRBYTE_ADDR    ,  x
voc_short GLOBAL_pp_ADDR          ,  x
voc_short GLOBAL_Q_ADDR           ,  x

//640*9
voc_short JPEGENC_BUFF_RGB_640_m1_ADDR[248]  ,  x
voc_short JPEGENC_BUFF_RGB_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_1_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_2_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_3_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_4_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_5_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_6_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_7_ADDR[640]  ,  x
voc_short JPEGENC_BUFF_RGB_640_8_ADDR[640]  ,  x



voc_alias  GLOBAL_TempZommBuf       JPEGENC_BUFF_RGB_640_m1_ADDR,X





voc_alias  GLOBAL_ORG_END_ADDR       JPEGENC_BUFF_RGB_ADDR+4224,X

voc_alias  GLOBAL_ORGI_CONST_ADDR    GLOBAL_ORG_END_ADDR,X
voc_alias  GLOBAL_ORGI_NUM_1_ADDR    GLOBAL_ORGI_CONST_ADDR    ,X//[1 SHORT]
voc_alias  GLOBAL_ORGI_DMASIZE_ADDR  GLOBAL_ORGI_CONST_ADDR+1  ,X//[1 SHORT]
voc_alias  GLOBAL_ORGI_DMAINC_ADDR   GLOBAL_ORGI_CONST_ADDR+2  ,X//[2 SHORT]
voc_alias  GLOBAL_SCALEX_SIZE_ADDR   GLOBAL_ORGI_CONST_ADDR+4  ,X//[2 SHORT]
voc_alias  GLOBAL_SCALEY_SIZE_ADDR   GLOBAL_ORGI_CONST_ADDR+6  ,X//[2 SHORT]
voc_alias  GLOBAL_MASK_ADDR          GLOBAL_ORGI_CONST_ADDR+8  ,X//[2 SHORT]
voc_alias  CONST_0XFF00FF_ADDR       GLOBAL_ORGI_CONST_ADDR+10 ,X//[2 SHORT]
voc_alias  GLOBAL_NUM1_ADDR          GLOBAL_ORGI_CONST_ADDR +12,x//[1 SHORT]
voc_alias  GLOBAL_NUM3_ADDR          GLOBAL_ORGI_CONST_ADDR +13,x//[1 SHORT]
voc_alias  GLOBAL_P1_ADDR            GLOBAL_ORGI_CONST_ADDR +14,x//[1 SHORT]
voc_alias  GLOBAL_P2_ADDR            GLOBAL_ORGI_CONST_ADDR +15,x//[1 SHORT]
voc_alias  GLOBAL_INPUT_ADDR_ADDR        GLOBAL_ORGI_CONST_ADDR + 16,x//[1 SHORT]
voc_alias  GLOBAL_INPUT_END_ADDR_ADDR    GLOBAL_ORGI_CONST_ADDR + 17,x//[1 SHORT]
voc_alias  GLOBAL_FlagForAverage_ADDR    GLOBAL_ORGI_CONST_ADDR + 18,x//[1 SHORT]
voc_alias  GLOBAL_ORGI_SOURCE_ADDR       GLOBAL_ORGI_CONST_ADDR + 20,X


//define allias//////////
voc_alias   AMR_EFR_RESET_Offset_X  144  ,x
voc_alias   STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR                 STRUCT_COD_AMRSTATE_ADDR  ,x
voc_alias   STRUCT_LEVINSONSTATE_OLD_A_ADDR                     STRUCT_LEVINSONSTATE_ADDR ,x
voc_alias   STRUCT_LSPSTATE_LSP_OLD_ADDR                        STRUCT_LSPSTATE_ADDR      ,x
voc_alias   STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR                     STRUCT_Q_PLSFSTATE_ADDR   ,x
voc_alias   STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR          STRUCT_CLLTPSTATE_ADDR    ,x
voc_alias   STRUCT_TONSTABSTATE_GP_ADDR                         STRUCT_TONSTABSTATE_ADDR  ,x
voc_alias   STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR                   STRUCT_DTX_ENCSTATE_ADDR  ,x
voc_alias   COD_AMR_XN_ADDRESS                                  BEGIN_OF_COD_AMR          ,x
voc_alias   PITCH_F6_EXCF_ADDR                                  AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR ,y
voc_alias   Pitch_fr6_max_loop_addr                             AMR_LOCAL_VARS_X_LEVEL1_START_ADDR ,x
voc_alias   Pitch_fr6_t0_min_addr                               (2+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   Pitch_delta_search_addr                             (4+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   Pitch_fr6_corr_v_addr                               (200+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR),x
voc_alias   G_PITCH_SCALED_Y1_ADDR                              AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR,y
voc_alias   G_PITCH_Y1_ADDR                                     (40+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR),y
voc_alias   COR_H_H2_ADDR                                       AMR_LOCAL_VARS_X_LEVEL1_START_ADDR,x
voc_alias   COR_H_H2_COPY_ADDR                                  (860+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR),y
voc_alias   COPY_C1035PF_SIGN_ADDR                              (900+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR),y
voc_alias   COR_H_X_Y32_ADDR                                    AMR_LOCAL_VARS_X_LEVEL1_START_ADDR,x
voc_alias   build_code_sign_ADDR                                AMR_LOCAL_VARS_X_LEVEL1_START_ADDR,x
voc_alias   cbsearch_dn_sign_ADDR                               C1035PF_SIGN_ADDR                 ,x
voc_alias   SET_SIGN_EN_ADDR                                    AMR_LOCAL_VARS_X_LEVEL1_START_ADDR,x

voc_alias   C1035PF_IPOS_D_ADDR                                 C1035PF_IPOS_ADDR                 ,x

voc_alias   DEC_AMR_SYNTH_ADDRESS                               (300+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR),y
voc_alias   COD_AMR_SYNTH_ADDR                                  DEC_AMR_SYNTH_ADDRESS                   ,y
voc_alias   DEC_TEMP_BEGIN_ADDR                                 (10+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x

voc_alias   F1_ADDR                                             (10+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) ,y
voc_alias   F2_ADDR                                             (22+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR) ,y
voc_alias   SYN_FILT_TMP_ADDR                                   AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR      ,y
voc_alias   INT_LPC_LSP_ADDR                                    AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR      ,y

voc_alias   BIT_SERIAL_ADDR_RAM                                 (20+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   BUILD_CODE_POINT_ADDR_ADDR                          AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR      ,y
voc_alias   AUTOCORR_Y_ADDR                                     (2+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)  ,x
voc_alias   AUTOCORR_Y_COPY_ADDR                                (4+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)  ,y
voc_alias   LEVINSON_AL_ADDR                                    (4+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)  ,y
voc_alias   PARA4_ADDR_ADDR                                     (3+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)  ,y
voc_alias   LEVINSON_ANL_ADDR                                   (242+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   LSP_LSP_NEW_Q_ADDR                                  (200+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   LSP_LSP_MID_ADDR                                    (210+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   LSP_LSP_MID_Q_ADDR                                  (220+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   AZ_LSP_F1_ADDR                                      (40+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)  ,x
voc_alias   AZ_LSP_F2_ADDR                                      (50+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)  ,x
voc_alias   AZ_LSP_F1_1_ADDR                                    (1+AZ_LSP_F1_ADDR)                       ,x
voc_alias   AZ_LSP_F2_1_ADDR                                    (1+AZ_LSP_F2_ADDR)                       ,x
voc_alias   CHEBPS_B2_ADDR                                      (10+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)  ,y
voc_alias   Q_PLSF_5_LSF_P_ADDR                                 (0+AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR)   ,y
voc_alias   PRE_BIG_AP1_ADDR                                    (10+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)  ,x
voc_alias   PRE_BIG_AP2_ADDR                                    (30+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)  ,x
voc_alias   PITCH_OL_M122_SCALED_SIGNAL2_ADDR                   (0+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR)   ,x
voc_alias   LPC_RLOW_ADDR                                       (300+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   LPC_RHIGH_ADDR                                      (312+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x
voc_alias   LPC_RC_ADDR                                         (330+AMR_LOCAL_VARS_X_LEVEL1_START_ADDR) ,x


//DEFINE CONST////////////
voc_alias   CONST_0_ADDR            25 + STATIC_CONST_INTER_6_25_ADDR,y
voc_alias   CONST_1_ADDR            1 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_2_ADDR            5 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_3_ADDR            6 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_4_ADDR            14 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_5_ADDR            15 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_6_ADDR            27 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_7_ADDR            28 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_8_ADDR            2 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_9_ADDR            4 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_10_ADDR            7 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_11_ADDR            13 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_12_ADDR            16 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_13_ADDR            26 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_14_ADDR            29 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_15_ADDR            42 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_16_ADDR            3 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_17_ADDR            8 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_18_ADDR            12 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_19_ADDR            17 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_20_ADDR            25 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_21_ADDR            30 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_22_ADDR            41 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_23_ADDR            43 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_24_ADDR            9 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_25_ADDR            11 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_26_ADDR            18 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_27_ADDR            24 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_28_ADDR            31 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_29_ADDR            40 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_30_ADDR            44 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_31_ADDR            53 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_32_ADDR            10 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_33_ADDR            19 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_34_ADDR            23 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_35_ADDR            32 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_36_ADDR            39 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_37_ADDR            45 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_38_ADDR            52 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_39_ADDR            54 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_40_ADDR            20 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_41_ADDR            22 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_42_ADDR            33 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_43_ADDR            38 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_44_ADDR            46 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_45_ADDR            51 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_46_ADDR            55 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_47_ADDR            60 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_48_ADDR            21 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_49_ADDR            34 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_50_ADDR            37 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_51_ADDR            47 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_52_ADDR            50 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_53_ADDR            56 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_54_ADDR            59 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_55_ADDR            61 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_56_ADDR            35 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_57_ADDR            36 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_58_ADDR            48 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_59_ADDR            49 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_60_ADDR            57 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_61_ADDR            58 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_62_ADDR            62 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_63_ADDR            63 + CONST_inverse_Zig_Zag_ADDR   ,x
voc_alias   CONST_0x2000_ADDR                   CONST_MYQUN_TAB1_ADDR  ,y
voc_alias   CONST_8192_ADDR_Y                   CONST_MYQUN_TAB1_ADDR  ,y
voc_alias   CONST_16384_ADDR                    1 + CONST_MYQUN_TAB1_ADDR  ,y
voc_alias   CONST_0x4000_ADDR                   CONST_16384_ADDR  ,y
voc_alias   CONST_0x1000_ADDR                   15 + CONST_MYQUN_TAB1_ADDR  ,y
voc_alias   CONST_4096_ADDR                     CONST_0x1000_ADDR     ,y
voc_alias   CONST_1024_ADDR                     CONST_MYQUN_TAB0_ADDR    ,y

voc_alias   STATIC_CONST_gray_ADDR                              STATIC_CONST_GRAY_ADDR  ,y
voc_alias   STATIC_CONST_gamma2_ADDR                            STATIC_CONST_F_GAMMA2_ADDR  ,y
voc_alias   STATIC_CONST_gamma1_12K2_ADDR                       STATIC_CONST_F_GAMMA1_ADDR  ,y
voc_alias   CONST_0x40000000L_ADDR                              CONST_0x40000000_ADDR       ,x
voc_alias   CONST_1_16_ADDR                                     CONST_D_1_16_ADDR           ,y
voc_alias   CONST_0x7fffffff_ADDR                               CONST_0x7FFFFFFF_ADDR       ,x
voc_alias   CONST_15565_ADDR                                    (     10+ STATIC_CONST_QUA_GAIN_PITCH_ADDR        )  ,y
voc_alias   CONST_13926_ADDR                                    (       8+ STATIC_CONST_QUA_GAIN_PITCH_ADDR       )  ,y
voc_alias   CONST_3277_ADDR                                     (     1+ STATIC_CONST_QUA_GAIN_PITCH_ADDR         )  ,y
voc_alias   CONST_0x87_ADDR                                     (     33+   STATIC_CONST_INTER_6_ADDR             )  ,y
voc_alias   CONST_9830_ADDR                                     (    4+ STATIC_CONST_QUA_GAIN_PITCH_ADDR          )  ,y
voc_alias   CONST_14746_ADDR                                    (   9+  STATIC_CONST_QUA_GAIN_PITCH_ADDR          )  ,y
voc_alias   CONST_0x3333_ADDR                                   (7+  STATIC_CONST_QUA_GAIN_PITCH_ADDR)  ,y
voc_alias   CONST_0x7fc5_ADDR                                   (225+  TABLE_WINDOW_232_8_ADDR)  ,y
voc_alias   CONST_1000_ADDR                                     (344+TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_80_ADDR                                       (316+TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_512_ADDR                                      (252+TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_0x5a82_ADDR                                   (16+ TABLE_LSP_LSF_ADDR)  ,y
voc_alias   CONST_0x00ff_ADDR                                   (484+ TABLE_DICO3_LSF_ADDR)  ,y
voc_alias   CONST_572_ADDR                                      (173+ TABLE_DICO5_LSF_ADDR )  ,y
voc_alias   CONST_500_ADDR                                      (356 + TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_1152_ADDR                                     (620+  TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_0xfff8_ADDR                                   (104 + TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_120_ADDR                                      (440+ TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_575_ADDR                                      (460+ TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_ALPHA_UP2_ADDR                                (540 +   TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_124_ADDR                                      (476+  TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_1311_ADDR                                     (814+  TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_368_ADDR                                      (222+  TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_0x235_ADDR                                    (247+  TABLE_DICO1_LSF_ADDR)  ,y
voc_alias   CONST_0x101_ADDR                                    (877+  TABLE_DICO4_LSF_ADDR)  ,y
voc_alias   CONST_97_ADDR                                       (394+  TABLE_DICO2_LSF_ADDR)  ,y
voc_alias   CONST_400_ADDR                                      (648+  TABLE_DICO4_LSF_ADDR)  ,y
voc_alias   CONST_0x00004000L_ADDR                              (48+TABLE_INV_SQRT_ADDR  )  ,y
voc_alias   CONST_0x00007FFF_ADDR                               (32+TABLE_LOG2_ADDR  )  ,y
voc_alias   CONST_0x00008000_ADDR                               (64+TABLE_LSP_LSF_ADDR )  ,y
voc_alias   CONST_RX_LOST_SID_FRAME_ADDR                        CONST_16_ADDR,x
voc_alias   CONST_RX_INVALID_SID_FRAME_ADDR                     CONST_32_ADDR,x
voc_alias   CONST_WORD32_0_ADDR                                 CONST_0_ADDR,y
voc_alias   CONST_0x00000001_ADDR                               CONST_1_ADDR,x
voc_alias   TABLE_hufftab0_ADDR                                 CONST_0x00000001_ADDR,x
voc_alias   CONST_0x7FFF_ADDR                                   CONST_0x00007FFF_ADDR,y
voc_alias   CONST_0x8000_ADDR                                   CONST_0x00008000_ADDR,y
voc_alias   CONST_0x00008000L_ADDR                              CONST_0x00008000_ADDR,y
voc_alias   CONST_0x0010_ADDR                                   CONST_16_ADDR,x
voc_alias   CONST_0x0011_ADDR                                   CONST_17_ADDR,x
voc_alias   CONST_0x0020_ADDR                                   CONST_32_ADDR,x
voc_alias   CONST_0x7fff_ADDR                                   CONST_0x7FFF_ADDR,y
voc_alias   CONST_0x400_ADDR                                    CONST_1024_ADDR,y
voc_alias   COSNT_1_2_ADDR                                      CONST_0x00004000L_ADDR,y
voc_alias   CONST_0x00007fffL_ADDR                              CONST_0x00007FFF_ADDR,y

voc_alias   CONST_NEG7_ADDR                                     629 + JPEGENC_MYDCT_ADDR,y
voc_alias   CONST_0xff_ADDR                                      CONST_0x00ff_ADDR,y

#endif


void COOLSAND_ENCODE_AMRRING(void)
{

    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i_short(REG0,2,IN_PARALLEL);

    VoC_lw16i_short(REG5,0,DEFAULT);//amr122-[2]
    VoC_lw16i_short(REG4,32,IN_PARALLEL);//length[byte]



    VoC_cfg_lg(CFG_WAKEUP_DMAI|CFG_WAKEUP_DMAE,CFG_WAKEUP_STATUS);
    VoC_cfg_lg(CFG_WAKEUP_DMAI|CFG_WAKEUP_DMAE,CFG_WAKEUP_MASK);

    VoC_sw32_d(REG45,Output_output_len_addr);
    VoC_sw16_d(REG0,Output_mode_addr);




    VoC_bez16_d(LABLE_VOC_INIT_FINISH,Input_reset_addr);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_sw16_d(REG2,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);
LABLE_VOC_INIT_FINISH:
    //dmai READ//////////////////////////////////////

    VoC_lw16i(REG7, INPUT_SPEECH_BUFFER_ADDR);
    VoC_shru16_ri(REG7,1,DEFAULT);//addr[int]
    VoC_lw16i_short(REG6,80,IN_PARALLEL);//size[int]
    VoC_lw32_d(ACC1,Input_inStreamBufAddr_addr);

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_lw16_d(REG2,GLOBAL_MIC_DIGITAL_GAIN_ADDR);  //

    VoC_lw16i_set_inc(REG0,INPUT_SPEECH_BUFFER_ADDR,1);

    VoC_lw16i_short(FORMAT16,(6-16),DEFAULT);

    VoC_loop_i(0,160)
    VoC_multf16_rp(REG3,REG2,REG0,DEFAULT);
    VoC_NOP();
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(FORMAT16,(15-16),DEFAULT);


    VoC_lw16_d(REG2,Input_reset_addr);
    VoC_lw16i_short(REG3,0,DEFAULT);// compute reset flag
    VoC_bez16_r(NO_SECTION_RESET,REG2);
    VoC_lw16i_short(REG3,1,DEFAULT);
NO_SECTION_RESET:
    VoC_or16_rd(REG2,GLOBAL_RELOAD_RESET_FLAG_ADDR);
    // keep reset flag & new mode for next turn
    VoC_sw16_d(REG3,GLOBAL_RELOAD_RESET_FLAG_ADDR);
    VoC_lw16i(REG0, INPUT_SPEECH_BUFFER_ADDR);      //INPUT buffer addr

    VoC_jal(CII_AMR_Encode);

    //DMAI WRITE////////////////////////////
    VoC_lw16i_short(REG2,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);
    VoC_lw32_d(ACC1,Input_outStreamBufAddr_addr);

    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_lw16i(REG6,320);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);
    VoC_sw32_d(REG67,Output_consumedLen_addr);
    VoC_sw16_d(ACC0_LO, Input_reset_addr );

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;

}
void  COOLSAND_RGB2JPEG()
{
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);


    VoC_sw16_d(REG2,CFG_DMA_WRAP);
    VoC_lw16i(STATUS,STATUS_CLR);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_sw32_d(ACC0,GLOBAL_input_p_Y_Row);
    VoC_sw32_d(ACC0,GLOBAL_input_p_Y_Colume);
    VoC_sw32_d(ACC0,GLOBAL_input_p_UV_Row);

    VoC_lw32_d(REG23,Input_inStreamBufAddr_addr);
    VoC_sw32_d(ACC0,GLOBAL_input_p_UV_Colume);
    VoC_sw32_d(REG23,GLOBAL_DmaInputBufferDddr);




    //////////select quality table//////////////////////
    VoC_be16_rd(LABLE_RESET_QUALITY_1,REG6,Input_JPEGENC_QUALITY_addr);
    VoC_lw16i(REG6,CONST_MYQUN_TAB0_ADDR); //LOW
    VoC_jump(LABLE_RESET_QUALITY_END);
LABLE_RESET_QUALITY_1:
    VoC_lw16i(REG6,CONST_MYQUN_TAB1_ADDR);//HIGH
LABLE_RESET_QUALITY_END:

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);//VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i(ACC1_LO,607);//length of head
    VoC_sw16_d(REG6,GLOBAL_Q_ADDR);//q_table
    VoC_sw32_d(ACC1,Output_output_len_addr);//encode length and mode
    ////////const prepare/////////////////////////////////
    VoC_sw16_d(ACC0_LO,GLOBAL_OLD_DCY_ADDR);
    VoC_sw32_d(ACC0,GLOBAL_OLD_DCU_ADDR);
    VoC_lw16i_short(ACC0_LO,8,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw32_d(ACC0,GLOBAL_SHORT_1_ADDR);
    exit_on_warnings = ON;
    VoC_sw32_d(ACC0,GLOBAL_CURRBITS_ADDR);


//  Y=(1225*R+2404*G+ 467*B)          >>12
//  u=(-691*R-1357*G+2048*B+128*4096) >>12
//  v=( 2048R-1715*G- 333*B+128*4096) >>12


    VoC_lw16i_set_inc(REG0,GLOBAL_RGBToYUV422Coef,1);
    VoC_lw16i(REG1,1225);
    VoC_lw16i(REG2,2404);
    VoC_lw16i(REG3,467);

    VoC_lw16i(REG4,-691);
    VoC_lw16i(REG5,-1357);
    VoC_lw16i(REG6,2048);

    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);

    VoC_lw16i(REG4,2048);
    VoC_lw16i(REG5,-1715);
    VoC_lw16i(REG6,-333);

    VoC_sw16inc_p(REG1,REG0,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);


    VoC_lw16_d(REG0,GlOBAL_ImgInw);
    VoC_lw16_d(REG1,GlOBAL_ImgInh);

    VoC_bne16_rd(COOLSAND_RGB2JPEG_L0,REG0,GlOBAL_ImgOutw)
    VoC_bne16_rd(COOLSAND_RGB2JPEG_L0,REG1,GlOBAL_ImgOuth)

    VoC_jal(COOLSAND_JpegEncodeWithoutZoom);

    VoC_jump(COOLSAND_RGB2JPEG_End);

COOLSAND_RGB2JPEG_L0:
    VoC_jal(COOLSAND_JpegEncodeWithZoom);
COOLSAND_RGB2JPEG_End:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
    /*
         /////////////////////////////////////////////////////////
         VoC_bez32_d(LABLE_STANDARD_JPEG,Input_samplerate_addr);
         VoC_jal(COOLSAND_ZOOM_JpgeEnc);
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return
         //////////
    LABLE_STANDARD_JPEG:

         VoC_lw16i(REG4,0x500);
         VoC_lw16i(REG5,0x400);
         VoC_bne32_rd(LABLE_NONE_1280BY1024,REG45,Input_JPEGENC_WIDTH_addr);
         VoC_bez16_d(LABLE_JPEG_RGB,Input_Channel_Num_addr);
         VoC_jal(COOLSAND_1280BY1024_YUV422_JPEG);
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return
    LABLE_JPEG_RGB:
         VoC_jal(COOLSAND_1280BY1024_JPEG);//VoC_jal(COOLSAND_1280BY1024_YUV422_JPEG);//
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return
    LABLE_NONE_1280BY1024:
         VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);
         VoC_lw16i(REG6,350);//411
         VoC_lw16i(REG7,700);//422 444
         VoC_blt16_rd(LABLE_JPEG_4222,REG6,Input_JPEGENC_WIDTH_addr);//if w<=560  411
         VoC_bez16_d(LABLE_JPEG_RGB_411,Input_Channel_Num_addr);
         VoC_jal(COOLSAND_YUV2JPEG411);
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return;
    LABLE_JPEG_RGB_411:
         VoC_jal(COOLSAND_RGB2JPEG411);//VoC_jal(COOLSAND_YUV2JPEG411);//
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return;
    LABLE_JPEG_4222:
         VoC_blt16_rd(LABLE_WRONG_SIZE,REG7,Input_JPEGENC_WIDTH_addr);//if w<=1120 422
         VoC_bez16_d(LABLE_JPEG_RGB_422,Input_Channel_Num_addr);
         VoC_jal(COOLSAND_YUVJPEG422);
         VoC_jump(LABLE_WRONG_SIZE);
    LABLE_JPEG_RGB_422:
         VoC_jal(COOLSAND_RGB2JPEG422);//VoC_jal(COOLSAND_YUVJPEG422);//
    LABLE_WRONG_SIZE:
         VoC_pop16(RA,DEFAULT);
         VoC_NOP();
         VoC_return;*/
}

void  COOLSAND_RGB2JPEG411()
{

    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-5,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,15);
    VoC_sw16_d(REG5,Output_reserve0_addr);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,Input_reserved_addr);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,Output_reserve0_addr);//whether the width has changed;
    VoC_sw16_d(REG4,Input_JPEGENC_WIDTH_addr);
LABLE_WIDTH_MULTIPLE16:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16,REG7);
    VoC_add16_rd(REG5,REG5,CONST_15_ADDR);
    VoC_and16_ri(REG5,0xfff0);
    VoC_NOP();
    VoC_sw16_d(REG5,Input_JPEGENC_HEIGHT_addr);
LABLE_WIDTH_HEIGHT_MULTIPLE16:
    VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
    VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
    VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8

    VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
    VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_411:
    VoC_lw16i_short(REG0,-4,DEFAULT);//16=2**4
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_411:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_jal(MCU_QUA_RGB2YUV411);


    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_MPY8);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+32,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_MPY8_ADD8);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+36,1);
    VoC_jal(MCU_QUA_RGB2YUV411);

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_411,REG5,Input_JPEGENC_WIDTH_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_411);
LABLE_CON1_411:
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_411,REG1,Input_JPEGENC_HEIGHT_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_411);
LABLE_CON2_411:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



void  MCU_QUA_RGB2YUV411(void)
{
    //push 16
    //martic RGB BUF   0

    //INPUT:
    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v

    //USED BUF:
    //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1

    //RETURN : NONE



    VoC_push16(REG1,DEFAULT);
    VoC_loop_i(1,4);

    VoC_loop_i(0,4);


    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG5,-5,DEFAULT);
    VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);

    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);//u
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);


    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru32_ri(ACC1,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16i(REG7,64);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0

    //////////////////////////////////////////
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  奇数行
    ////////////////////////////////////////////
    VoC_loop_i(0,4);

    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);



    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_lw16_sd(REG1,0,IN_PARALLEL);//REWIN R1
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to  Y_BUF
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);
    VoC_lw16i_short(REG4,4,DEFAULT);//FOR U/V INC
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  偶数行
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);

    VoC_endloop1

    VoC_pop16(REG1,DEFAULT);//MATRIC

    VoC_return;



}

////////////////////////////////////////////////
void  COOLSAND_RGB2JPEG422()
{
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-4,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,7);
    VoC_sw16_d(REG5,Output_reserve0_addr);
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_422,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,Input_reserved_addr);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,Output_reserve0_addr);//whether the width has changed;
    VoC_sw16_d(REG4,Input_JPEGENC_WIDTH_addr);
LABLE_WIDTH_MULTIPLE16_422:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_422,REG7);
    VoC_add16_rd(REG5,REG5,CONST_7_ADDR);
    VoC_and16_ri(REG5,0xfff8);
    VoC_NOP();
    VoC_sw16_d(REG5,Input_JPEGENC_HEIGHT_addr);
LABLE_WIDTH_HEIGHT_MULTIPLE16_422:
    VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
    VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
    VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
    VoC_lw16i_short(REG1,0,IN_PARALLEL);
    //VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8

    VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
    VoC_shr16_ri(REG4,-1,DEFAULT);//VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_422:
    VoC_lw16i_short(REG0,-3,DEFAULT);//8=2**3
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_422:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_jal(MCU_QUA_RGB2YUV422);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);
    VoC_jal(MCU_QUA_RGB2YUV422);

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_422,REG5,Input_JPEGENC_WIDTH_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_422);
LABLE_CON1_422:
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_422,REG1,Input_JPEGENC_HEIGHT_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_422);
LABLE_CON2_422:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}



void  MCU_QUA_RGB2YUV422(void)
{
    //push 16
    //martic RGB BUF   0

    //INPUT:
    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v

    //USED BUF:
    //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1

    //RETURN : NONE



    VoC_push16(REG1,DEFAULT);
    VoC_loop_i(1,8);

    VoC_loop_i(0,4);


    //1st point
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru16_ri(REG5,-5,DEFAULT);
    VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);

    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);//u
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);


    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_shru32_ri(ACC1,-7,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
    VoC_lw16_sd(REG1,0,IN_PARALLEL);
    VoC_shru32_ri(ACC1,8,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    //the 2nd point
    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);
    VoC_shru16_ri(REG5,10,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);



    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_lw16i(REG7,64);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
    exit_on_warnings = ON;
    VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);
    VoC_inc_p(REG3,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);//write to Y_BUF

    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行  奇数行

    VoC_endloop1

    VoC_pop16(REG1,DEFAULT);//MATRIC

    VoC_return;



}

////////////////////////////////////////////////
//void  COOLSAND_RGB2JPEG444()
//{
//  VoC_push16(RA,DEFAULT);
//  VoC_lw16_d(REG4,Input_JPEGENC_WIDTH_addr);
//  VoC_add16_rd(REG6,REG4,CONST_8_ADDR);//W+8
//  VoC_sub16_rd(REG7,REG4,CONST_8_ADDR);//W-8
//  VoC_shr16_ri(REG4,-3,DEFAULT);//W*8
//  VoC_lw16i_short(REG1,0,IN_PARALLEL);
//  //VoC_add16_rd(REG5,REG4,CONST_8_ADDR);//W*8+8
//
//  VoC_sw32_d(REG67,CONST_WIDTH_ADD8);
//  VoC_shr16_ri(REG4,-1,DEFAULT);//VoC_shr16_ri(REG4,-2,DEFAULT);
//  VoC_lw16i_short(REG5,0,IN_PARALLEL);
//  exit_on_warnings = OFF;
//  VoC_sw32_d(REG45,CONST_WIDTH_MPY8);
//  exit_on_warnings = ON;
//  VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
//
//   ///////////////////////////////////////////////////
//LABLE_REG2JPEG_LOOP_HEIGHT_444:
//  VoC_jal(DEMAI_READ_RGBDATA);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,1);
//  VoC_lw16i_short(REG5,0,DEFAULT);
//LABLE_REG2JPEG_LOOP_WIDTH_444:
//    VoC_lw16i_short(INC0,1,DEFAULT);
//  VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
//
//    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
//  VoC_push16(REG5,IN_PARALLEL);//width loop
//    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
//  VoC_jal(MCU_QUA_RGB2YUV444);
//
//
//  //ENOCDE BLOCK
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_short(REG4,0,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);
//
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_lw16i_short(REG4,1,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);
//
//  VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
//  VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
//  VoC_lw16i_short(REG4,1,DEFAULT);
//  VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
//
//  ////////////////////////////////
//  VoC_pop32(REG01,DEFAULT);
//    VoC_pop16(REG5,IN_PARALLEL);
//  VoC_lw16i_short(REG4,8,DEFAULT);
//  VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
//    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
//  /////////////////////////////////
//  VoC_bnlt16_rd(LABLE_CON1_444,REG5,Input_JPEGENC_WIDTH_addr);
//  VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_444);
//LABLE_CON1_444:
//  VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
//    VoC_bnlt16_rd(LABLE_CON2_444,REG1,Input_JPEGENC_HEIGHT_addr);
//  VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_444);
//LABLE_CON2_444:
//  ////////////////////////////////////////////////////////////////////
//  VoC_jal(COOLSNAD_flush_bits);
//  //whether there is  odd byte left
//    VoC_pop16(RA,DEFAULT);
//  VoC_NOP();
//
//  VoC_return;
//}
//
//
//
//void  MCU_QUA_RGB2YUV444(void)
//{
//  //push 16
//  //martic RGB BUF   0
//
//  //INPUT:
//    //r0:rgb buf  r1:CONST BUF(INT RAM_X) r2:y   r3:u v
//
//  //USED BUF:
//  //R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1
//
//  //RETURN : NONE
//
//
//
//  VoC_push16(REG1,DEFAULT);
//  VoC_loop_i(1,8);
//
//  VoC_loop_i(0,8);
//
//
//      //1st point
//     VoC_lw16inc_p(REG4,REG0,DEFAULT);
//     VoC_lw32z(ACC1,IN_PARALLEL);
//     VoC_movreg16(REG6,REG4,DEFAULT);
//     VoC_movreg16(REG5,REG4,IN_PARALLEL);
//     VoC_shru16_ri(REG4,11,DEFAULT);
//     VoC_lw16_sd(REG1,0,IN_PARALLEL);
//     VoC_shru16_ri(REG5,-5,DEFAULT);
//     VoC_shru16_ri(REG4,-3,IN_PARALLEL);//r
//     VoC_shru16_ri(REG5,10,DEFAULT);
//     VoC_shru16_ri(REG6,-11,IN_PARALLEL);
//
//
//
//     //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
//     VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
//     VoC_shru16_ri(REG5,-2,IN_PARALLEL);//g
//     VoC_mac32inc_rp(REG5,REG1,DEFAULT);
//     VoC_shru16_ri(REG6,8,IN_PARALLEL);//b
//     VoC_mac32inc_rp(REG6,REG1,DEFAULT);
//     VoC_movreg16(ACC1_LO,REG6,DEFAULT);//save b to acc1
//     VoC_shru32_ri(ACC0,8,DEFAULT);
//     VoC_multf32inc_rp(ACC0,REG4,REG1,IN_PARALLEL);
//       exit_on_warnings = OFF;
//     VoC_mac32inc_rp(REG5,REG1,DEFAULT);
//     VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
//       exit_on_warnings = ON;
//     VoC_sub16_rd(REG7,REG7,CONST_128_ADDR);
//
//     //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
//     VoC_shru32_ri(ACC1,-7,DEFAULT);
//     VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
//     VoC_lw32z(ACC1,IN_PARALLEL);//u
//     VoC_sw16inc_p(REG7,REG2,DEFAULT);//write into Y_BUF1[0]
//     VoC_shru32_ri(ACC0,8,IN_PARALLEL);
//
//
//     //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
//     VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
//     VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
//     exit_on_warnings = OFF;
//     VoC_mac32_rp(REG6,REG1,DEFAULT);
//     VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
//     exit_on_warnings = ON;
//     VoC_shru32_ri(ACC1,-7,DEFAULT);
//       VoC_lw16i_short(REG7,64,IN_PARALLEL);
//     VoC_add32_rr(ACC1,ACC0,ACC1,DEFAULT);
//     VoC_lw16_sd(REG1,0,IN_PARALLEL);
//     VoC_shru32_ri(ACC1,8,DEFAULT);
//       VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);
//     VoC_inc_p(REG3,DEFAULT);
//     VoC_sw16_p(ACC1_LO,REG7,DEFAULT);//write to V_BUF
//
//     VoC_endloop0
//
//     VoC_add16_rd(REG0,REG0,CONST_WIDTH_SUB8);//换行
//
//     VoC_endloop1
//
//     VoC_pop16(REG1,DEFAULT);//MATRIC
//
//     VoC_return;
//
//}
//process 1280*1024 size image

void COOLSAND_Double2Single()
{
    //r1:pin    r0:pout
    //used buf: r0 r1 r2 r4 r5 r6 r7 rl6

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG4,11,IN_PARALLEL);

    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0

    VoC_push16(REG0,DEFAULT);//caution???
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);//save r

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//r1g1b1

    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b
    VoC_movreg16(REG6,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG7,REG6,REG2,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL)

    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);

    VoC_endloop0

    VoC_return;

}

void COOLSAND_Double2SingleLastRow()
{
    //r1:pin    r0:pout
    //used buf: r0 r1 r2 r4 r5 r6 r7 rl6

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);

    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_loop_i_short(7,DEFAULT);
    VoC_startloop0

    VoC_push16(REG0,DEFAULT);//caution???
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);//save r

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//r1g1b1

    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_movreg16(REG5,REG4,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b
    VoC_movreg16(REG6,REG4,IN_PARALLEL);

    VoC_shru16_ri(REG4,11,DEFAULT);
    VoC_shru16_ri(REG5,-5,IN_PARALLEL);

    VoC_shru16_ri(REG4,-3,DEFAULT);//r
    VoC_shru16_ri(REG5,10,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_shru16_ri(REG5,-2,DEFAULT);//g
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//b

    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_movreg32(RL6,REG45,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG7,REG6,REG2,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL)

    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_movreg32(REG45,RL6,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);

    VoC_endloop0


    VoC_sw16inc_p(REG4,REG0,DEFAULT);//save r
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b

    VoC_sw16inc_p(REG4,REG0,DEFAULT);//save r
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//save g
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//save b

    VoC_return;
}
void COOLSAND_Double2Row()
{
    //in1:r2 INC2=2 ;  in2:r3 INC3=1 ; out:r0  INC0=2;
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_lw32inc_p(REG67,REG2,DEFAULT);

    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//r
    VoC_add16inc_rp(REG6,REG6,REG3,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);//g

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG0,DEFAULT);//save rg
    VoC_shru16_ri(REG6,1,IN_PARALLEL);//b
    exit_on_warnings=ON;

    VoC_add16inc_rp(REG7,REG7,REG3,DEFAULT);//r
    VoC_add16inc_rp(REG4,REG4,REG3,DEFAULT);//g
    VoC_shru16_ri(REG7,1,IN_PARALLEL);
    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);//b
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);//save br
    VoC_sw32inc_p(REG45,REG0,DEFAULT);//save gb

    VoC_endloop0

    VoC_return;
}

void COOLSAN_Encode422MCUBlock()
{
    //r0:rgb buf(Y)  r1:CONST BUF(INT RAM_Y)   r2:y   r3:u v(X)
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);


    VoC_loop_i(1,8);
    VoC_loop_i(0,4);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//R
    VoC_push16(REG1,IN_PARALLEL);//X

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//G
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//B
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);//save b to acc1
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,-7,DEFAULT);//b << 7
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);//(r * -43)
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);//(g * -85)
    VoC_sw16inc_p(ACC0_LO,REG2,IN_PARALLEL);//write into Y_BUF1[0]
    exit_on_warnings=ON;
    VoC_lw16i(REG7,64);
    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_lw16inc_p(REG4,REG0,DEFAULT);//r
    VoC_shru32_ri(ACC1,-7,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//g
    VoC_lw16_sd(REG1,0,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//b
    VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//v
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,8,IN_PARALLEL);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);

    VoC_inc_p(REG3,DEFAULT);//op
    VoC_sw16_p(ACC1_LO,REG7,IN_PARALLEL);//write to V_BUF

    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);//write to Y_BUF
    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_24_ADDR);//换行
    VoC_endloop1

    //////////////////////////////////////////////////////////
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR+24,1);
    VoC_lw16i_set_inc(REG1,MATRIC_RGB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR+4,1);

    VoC_loop_i(1,8);
    VoC_loop_i(0,4);

    VoC_lw16inc_p(REG4,REG0,DEFAULT);//R
    VoC_push16(REG1,IN_PARALLEL);//X

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//G
    VoC_lw32z(ACC1,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//B
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);//save b to acc1
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,-7,DEFAULT);//b << 7
    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);//(r * -43)
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);//(g * -85)
    VoC_sw16inc_p(ACC0_LO,REG2,IN_PARALLEL);//write into Y_BUF1[0]
    exit_on_warnings=ON;
    VoC_lw16i(REG7,64);
    //u = (((r * -43) + (g * -85) + (b << 7)) >> 8);
    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_shru32_ri(ACC0,8,DEFAULT);
    //V = (((r << 7) + (g * -107) + (b * -21)) >> 8)
    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_movreg16(ACC1_LO,REG4,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_mac32_rp(REG6,REG1,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG3,IN_PARALLEL);//write into U_BUF
    exit_on_warnings = ON;
    VoC_lw16inc_p(REG4,REG0,DEFAULT);//r
    VoC_shru32_ri(ACC1,-7,IN_PARALLEL);

    VoC_lw16inc_p(REG5,REG0,DEFAULT);//g
    VoC_lw16_sd(REG1,0,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);//b
    VoC_add32_rr(ACC1,ACC0,ACC1,IN_PARALLEL);//v
    //y = (((r * 76) + (g * 151) + (b * 29)) >> 8) - 128;
    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_shru32_ri(ACC1,8,IN_PARALLEL);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);

    VoC_inc_p(REG3,DEFAULT);//op
    VoC_sw16_p(ACC1_LO,REG7,IN_PARALLEL);//write to V_BUF

    VoC_shru32_ri(ACC0,8,DEFAULT);
    VoC_sub32_rd(ACC0,ACC0,CONST_0X80_ADDR);

    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_short(REG4,4,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);//write to Y_BUF
    VoC_endloop0
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_add16_rd(REG0,REG0,CONST_24_ADDR);//换行
    VoC_endloop1
    ///////////////////////////////////////////////////////
    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;


}
void COOLSAND_MEMCopy()
{
    //r2:in   r3:out
    //used  buf: r2 r3 RL6 rl7
    VoC_loop_i_short(12,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(RL6,REG2,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;

}

void COOLSAND_DMAIRead_1280BY1024Even()
{
    //INPUT:R4 LINE1 ,r5 line2
    //USED BUF: R6 R7 ACC0 FORMAT32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i(REG7,(JPEGENC_BUFF_RGB_ADDR)/2);//IN addr[INT]
    VoC_multf32_rd(ACC0,REG4,CONST_640_ADDR);//[byte]
    VoC_lw16_d(REG6,CONST_640BY8_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,Input_inStreamBufAddr_addr);//out [BYTE]

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}

void COOLSAND_DMAIRead_1280BY1024Odd()
{
    //INPUT:R4 LINE1 ,r5 line2
    //USED BUF: R6 R7 ACC0 FORMAT32
    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
    VoC_lw16i(REG7,(JPEGENC_BUFF_RGB_ADDR)/2);//IN addr[INT]
    VoC_multf32_rd(ACC0,REG4,CONST_640_ADDR);//640*LINE1

    VoC_bnlt16_rd(LABLE_BIGTHEN479,REG5,CONST_479_ADDR);
    VoC_lw16_d(REG6,CONST_640BY9_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,Input_inStreamBufAddr_addr);//out [BYTE]

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
LABLE_BIGTHEN479:
    VoC_lw16_d(REG6,CONST_640BY8_INT_ADDR);//size [int]
    VoC_add32_rd(ACC0,ACC0,Input_inStreamBufAddr_addr);//out [BYTE]

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    ///////////////////////////////////////////////////////////////
    VoC_multf32_rd(ACC0,REG5,CONST_640_ADDR);
    VoC_lw16_d(REG6,CONST_640_INT_ADDR);//size [int]
    VoC_lw16i(REG7,(JPEGENC_BUFF_RGB_640_8_ADDR)/2);//IN Addr[INT]
    VoC_add32_rd(ACC0,ACC0,Input_inStreamBufAddr_addr);//out [BYTE]

    VoC_sw32_d(REG67,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;
}

void COOLSAND_1280BY1024_JPEG()
{
#if 0
    voc_short TEMP_BUFF_FOR1280BY1024_TMP1[48]  ,  y
    voc_short TEMP_BUFF_FOR1280BY1024_TMP2[48]  ,  y
#endif

    VoC_lw16i_short(REG5,7,DEFAULT);//line2
    VoC_lw16i_short(REG4,0,IN_PARALLEL);//line1

    VoC_push16(RA,DEFAULT);
LABLE_START_1280BY1024:
    VoC_push32(REG45,DEFAULT);//line2  line1
    //even-16   first-8
    VoC_lw16i_short(REG0,79,IN_PARALLEL);//count=79;

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_ADDR,1);//pin_addr
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Even);
LABLE_DO_BEGIN_FRONT_EVEN:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r0:pout
    VoC_jal(COOLSAND_Double2Single);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//int
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);      //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2Single);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    VoC_jal(COOLSAN_Encode422MCUBlock);


    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW1,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_EVEN);
LABLE_LASTROW1:
    //last row
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r2:pout
    VoC_jal(COOLSAND_Double2SingleLastRow);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);              //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    VoC_jal(COOLSAN_Encode422MCUBlock);
    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_3_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;

LABLE_DO_BEGIN_BEHIND_EVEN:
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    //奇数行
    VoC_jal(COOLSAND_Double2Single);


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2Single);

    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW2,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_EVEN);
LABLE_LASTROW2:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    //奇数行
    //double2single(8,pin+640*3,bufftmp);//7
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640*4,pout+48);//9
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_48_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow);
    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_3_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_5_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_7_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    //ODD-16 FIRST-8
    VoC_lw32_sd(REG45,0,DEFAULT);//LINE2 LINE1
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//      line1+=7;
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);//  line2+=7;
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Odd);
    VoC_sw32_sd(REG45,0,DEFAULT);//save line2 line1
    VoC_lw16i_short(REG0,79,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_ADDR,1);
LABLE_DO_BEGIN_FRONT_ODD:
    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2Single);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW3,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_ODD);
LABLE_LASTROW3:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_4_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;
LABLE_DO_BEGIN_BEHIND_ODD:
    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2Single);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW4,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_ODD);
LABLE_LASTROW4:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_2_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_4_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_48_6_ADDR,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_48_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_48_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);
    //update line1 and line2
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_bgt16_rd(LABLE_END_1280BY1024,REG5,CONST_479_ADDR);
    VoC_jump(LABLE_START_1280BY1024);
LABLE_END_1280BY1024:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;


}


////////////////////////////////////////////////
void DEMAI_READ_RGBDATA()
{
    /*  DMA_I
       IN_ADDR : INT
       SIZE:     INT
       EX_ADDR:  BYTE
    */
    VoC_lw32_d(ACC0,Input_inStreamBufAddr_addr);//[BYTE]
    VoC_lw16i(REG6,JPEGENC_BUFF_RGB_ADDR);//VoC_shr16_ri(REG0,1,IN_PARALLEL);//dmai-addr[int]
    VoC_shr16_ri(REG6,1,DEFAULT);//[int]
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);


    VoC_lw32_d(REG23,CONST_BLOCKSIZE_BYTE_ADDR);//SIZE [BYTE]  32-BIT
    VoC_add32_rr(RL6,REG23,ACC0,DEFAULT);//NEW EX_ADDR [BYTE]
    VoC_shr16_ri(REG2,2,IN_PARALLEL);//SIZE [INT]
    ////////dmai//////////////////
    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG2,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_sw32_d(RL6,Input_inStreamBufAddr_addr);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    /////////////////////////////////////////////////////////
    VoC_bnez16_d(LABLE_NONE_MULTIPLE,Output_reserve0_addr);
    VoC_return;
LABLE_NONE_MULTIPLE:
    VoC_lw16_d(REG2,Input_reserved_addr);//original width
    VoC_lw16_d(REG3,Input_JPEGENC_WIDTH_addr);//new width

    VoC_sub16_rr(REG4,REG3,REG2,DEFAULT);
    VoC_shru16_rr(REG6,REG0,IN_PARALLEL);//2**N

    VoC_shru16_rr(REG2,REG0,DEFAULT);//16*original width  or 8*original width
    VoC_shru16_rr(REG3,REG0,IN_PARALLEL);//16*new width  or 8*new width

    VoC_lw16i(REG5,JPEGENC_BUFF_RGB_ADDR)
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);


    VoC_sub16_rr(REG2,REG2,REG7,DEFAULT);//last
    VoC_sub16_rr(REG3,REG3,REG7,IN_PARALLEL);//last
    VoC_sub16_rr(REG0,REG6,REG7,DEFAULT);


    VoC_lw16i(REG5,254);
    VoC_loop_r(1,REG0);//num of rows - 1
    VoC_movreg32(ACC0,REG23,DEFAULT);
    VoC_lw16i(REG7,127);
    VoC_lw16_d(REG6,Input_reserved_addr);//original width
LABLE_LOOP0_START:
    VoC_bgt16_rr(LABLE_WIDTH_255,REG6,REG5);
    VoC_add16_rd(REG7,REG6,CONST_1_ADDR);
    VoC_shru16_ri(REG7,1,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_jump(LABLE_LOOP_BEGIN);
LABLE_WIDTH_255:
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);
LABLE_LOOP_BEGIN:
    VoC_loop_r(0,REG7)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG0,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG0,REG3,DEFAULT);
    VoC_endloop0
    VoC_bnez16_r(LABLE_LOOP0_START,REG6);
    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_sub16_rd(REG2,REG2,Input_reserved_addr);
    VoC_sub16_rd(REG3,REG3,Input_JPEGENC_WIDTH_addr);
    VoC_endloop1

    VoC_return;
}


/* encode_block:
 *  Encodes an 8x8 basic block of coefficients of given type (luminance or
 *  chrominance) and writes it to the output stream.
 */
void COOLSAND_encode_block(void)
{

    //input:r4:type  r2:old_dc addr
    //output:write old_dc  and  encoded  information into ram
    //all buf have been used

    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);//data  ONLY R0
    VoC_bne16_rd(LABLE_ENCODEBLOCK_TYPE1,REG4,CONST_0_ADDR);//if (type ==0)
    VoC_lw16i(REG0,CONST_MYHUFF_DC0_ADDR);//dc
    VoC_lw16i_set_inc(REG1,CONST_MYHUFF_AC0_ADDR+2,-2);//ac
    VoC_jump(LABLE_ENCODEBLOCK_TYPE_END);
LABLE_ENCODEBLOCK_TYPE1://  else if (type==1)
    VoC_lw16i(REG0,CONST_MYHUFF_DC1_ADDR);//dc
    VoC_lw16i_set_inc(REG1,CONST_MYHUFF_AC1_ADDR+2,-2);//ac
LABLE_ENCODEBLOCK_TYPE_END:

    // VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);

    VoC_push16(REG2,DEFAULT);//old_dcy/u/v addr
    VoC_lw16i_short(REG6,64,IN_PARALLEL);

    VoC_mult16_rr(REG6,REG6,REG4,DEFAULT);//64*type
    VoC_pop32(REG01,IN_PARALLEL);//data  ONLY R0
    exit_on_warnings = OFF;
    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);//prepare r4=1 for apply_fdct
    exit_on_warnings = ON;
    VoC_add16_rd(REG2,REG6,GLOBAL_Q_ADDR);//pTempQuanTab = quant_table + 64*type;


    VoC_jal(apply_fdct);//apply_fdct(data);//r2 is not used in this fun


    VoC_pop16(REG1,DEFAULT);//GLOBAL_OLD_DCY_ADDR
    VoC_lw16i_short(FORMAT16,1,IN_PARALLEL);//>>17

    VoC_push16(REG0,DEFAULT);//boloc[0] addr

    VoC_multf16inc_pp(REG4,REG0,REG2,DEFAULT);//dc_tmp = MPY_16_16_l19(block[0],pTempQuanTab[0]);//dc_tmp = (block[0] * quant_table[0]) >> 19;

    VoC_lw16i_short(INC0,1,DEFAULT);
    //VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_sub16_rp(REG4,REG4,REG1,DEFAULT);//value = dc_tmp - *old_dc;
    exit_on_warnings = OFF;
    VoC_sw16_p(REG4,REG1,DEFAULT);//*old_dc = dc_tmp;
    exit_on_warnings = ON;
    VoC_pop32(REG01,IN_PARALLEL);//r0:dc addr   r1:ac addr




    VoC_jal(format_number);//input:r4     //output: r0:Addr  r4:cat   r5:bits
    //unsed buf: r1 r2

    //put_bits(dc_table->code[category]->encoded_value,dc_table->code[category]->bits_length);
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);//r0: ac addr
    VoC_push16(REG1,DEFAULT);//ac_table
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//num_zeroes = 0;
    VoC_lw16i_set_inc(REG1,CONST_inverse_Zig_Zag_ADDR+1,1);
    VoC_jal(COOLSAND_put_bits);
    VoC_jal(COOLSAND_put_bits);
    //put_bits(REGS[5].reg, REGS[4].reg);

    VoC_lw16_sd(REG5,1,DEFAULT);//value = block[inverse_Zig_Zag[index]];
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);//value addr
    //r1:inverse_Zig_Zag
    //r2:pTempQuanTab
    //r3:num_zeroes
    VoC_loop_i(1,63);//for (index = 1; index < 64; index++)

    VoC_lw16i_short(FORMAT16,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16_p(REG6,REG5,DEFAULT);//value
    VoC_multf16inc_pp(REG4,REG5,REG2,IN_PARALLEL);
    VoC_shru16_ri(REG6,15,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//value = MPY_16_16_l19((short)value,pTempQuanTab[index]) + ((value >> 31) & 0x1);// value = ((value * quant_table[index]) >> 19) + ((value >> 31) & 0x1);
    VoC_lw16i_short(FORMAT16,-16,IN_PARALLEL);

    VoC_bne16_rd(LABLE_ENCODEBLOCK_num_zeroes_bgin,REG4,CONST_0_ADDR);//if (value == 0)
    VoC_inc_p(REG3,DEFAULT);//num_zeroes++;
    VoC_jump(LABLE_ENCODEBLOCK_VALUE_END);
LABLE_ENCODEBLOCK_num_zeroes_bgin:


    VoC_bngt16_rd(LABLE_ENCODEBLOCK_num_zeroes,REG3,CONST_15_ADDR);//while (num_zeroes > 15)
    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_jal(COOLSAND_put_bits);
    //put_bits(ac_table->code[0xf0]->encoded_value,ac_table->code[0xf0]->bits_length);
    VoC_sub16_rd(REG3,REG3,CONST_16_ADDR);//num_zeroes -= 16;
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_bgt16_rd(LABLE_ENCODEBLOCK_num_zeroes_bgin,REG3,CONST_15_ADDR);

LABLE_ENCODEBLOCK_num_zeroes:


    VoC_jal(format_number);//format_number(value, &category, &bits);
    VoC_add16_rr(REG0,REG3,REG0,DEFAULT);//ac addr
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//num_zeroes = 0;

    VoC_lw16_sd(REG0,0,DEFAULT);//VoC_movreg16(REG0,REG1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_lw32_p(REG67,REG0,DEFAULT);
    exit_on_warnings = ON;
    //put_bits(ac_table->code[value]->encoded_value,ac_table->code[value]->bits_length);
    VoC_jal(COOLSAND_put_bits);
    VoC_jal(COOLSAND_put_bits);


LABLE_ENCODEBLOCK_VALUE_END:
    VoC_lw16i_short(INC0,-2,DEFAULT);
    VoC_lw16_sd(REG5,1,IN_PARALLEL);//value = block[inverse_Zig_Zag[index]];
    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT);//value addr
    VoC_endloop1


    VoC_inc_p(REG0,DEFAULT);//r0-2
    VoC_pop16(REG6,IN_PARALLEL);//AC_ADDR

    VoC_pop16(REG6,DEFAULT);//BLOCK

    VoC_bngt16_rd(LABLE_ENCODEBLOCK_RETURN,REG3,CONST_0_ADDR);//if (num_zeroes > 0)
    VoC_lw32_p(REG67,REG0,DEFAULT);//put_bits(ac_table->code[0x0]->encoded_value,ac_table->code[0x0]->bits_length);
    VoC_jal(COOLSAND_put_bits);
LABLE_ENCODEBLOCK_RETURN:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_return;
//  return 0;
}
/* apply_fdct:
 *  Applies the forward discrete cosine transform to the given input block,
 *  in the form of a vector of 64 coefficients.
 *  This uses integer fixed point math and is based on code by the IJG.
 */
void apply_fdct(void)
{
#if 0
    voc_short TEMP_BUFF_64_ADDR[32] ,y
#endif
    //input:data// VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,2);
    //output:data
    //used buf: reg0 reg1 reg3 reg4 reg5 reg6 reg7  acc0 acc1 rl6 rl7
    //unused buf:reg2
    /////////////////////////////////////////////////////////////////////
    VoC_lw16i_set_inc(REG1,TEMP_BUFF_64_ADDR,1);
    //VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_push32(REG01,DEFAULT);//o
    exit_on_warnings = ON;
    VoC_loop_i_short(16,DEFAULT);//64 take the even element to the temp_buff_64_addr
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);//even part
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings = ON;

    VoC_endloop0

    VoC_lw32_sd(REG01,0,DEFAULT);//VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,2);//even
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,TEMP_BUFF_64_ADDR+3,-1);//odd
    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);

    VoC_loop_i(0,8);//for (i = 8; i; i--)
    //VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);
    //VoC_startloop0
    VoC_add16inc_pp(REG4,REG1,REG0,DEFAULT);//tmp0 = dataptr[0] + dataptr[7];
    VoC_lw16i_short(FORMAT32,-16+11,IN_PARALLEL);
    VoC_add16inc_pp(REG5,REG1,REG0,DEFAULT);//tmp2 = dataptr[2] + dataptr[5];
    VoC_add16inc_pp(REG6,REG1,REG0,DEFAULT);//tmp3 = dataptr[4] + dataptr[3];
    VoC_add16_pp(REG7,REG1,REG0,DEFAULT);//tmp1 = dataptr[6] + dataptr[1];
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//tmp10 = tmp0 + tmp3;
    VoC_sub16_rr(REG6,REG4,REG6,IN_PARALLEL);//tmp13 = tmp0 - tmp3;

    VoC_add16_rr(REG5,REG7,REG5,DEFAULT);//tmp11 = tmp1 + tmp2;
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);//tmp12 = tmp1 - tmp2;

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//dataptr[0] = (tmp10 + tmp11) << 2;
    VoC_sub16_rr(REG5,REG4,REG5,IN_PARALLEL);//dataptr[4] = (tmp10 - tmp11) << 2;

    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_shr16_ri(REG5,-2,IN_PARALLEL);

    //z1 = MPY_16_16((short)(tmp12 + tmp13) , 4433);
    VoC_movreg32(RL6,REG45,DEFAULT);//RL6:0 4
    VoC_add16_rr(REG4,REG7,REG6,IN_PARALLEL);//(tmp12 + tmp13)

    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);//z1
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);
    /////////////////////////////////////////////////
    //dataptr[2] = (z1 + MPY_16_16(tmp13 ,6270)) >> 11;
    //VoC_lw16i(REG3,6270);
    VoC_mac32inc_rp(REG6,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG3,IN_PARALLEL);//dataptr[6] = (z1 + MPY_16_16(tmp12 , -15137)) >> 11;
    exit_on_warnings = ON;
    //////////////////////////////////////////////////////////////////
    VoC_sub16inc_pp(REG6,REG1,REG0,DEFAULT);//tmp6 = dataptr[1] - dataptr[6];
    VoC_sub16inc_pp(REG4,REG1,REG0,DEFAULT);//tmp4 = dataptr[3] - dataptr[4];
    VoC_sub16inc_pp(REG5,REG0,REG1,DEFAULT);//tmp5 = dataptr[2] - dataptr[5];
    VoC_sub16_pp(REG7,REG0,REG1,DEFAULT);//tmp7 = dataptr[0] - dataptr[7];
    VoC_lw16i_short(INC0,2,IN_PARALLEL);


    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//z1 = tmp4 + tmp7;
    VoC_push16(REG4,IN_PARALLEL);//tmp4
    /////write dataptr 0 2 4 6/////////////////OP
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);//0
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);//z1 *= -7373;
    VoC_pop16(REG4,DEFAULT);//tmp4
    VoC_mac32inc_rp(REG4,REG3,DEFAULT);//tmp4 *= 2446; -> tmp4 + z1->acc0
    VoC_sw16inc_p(ACC0_HI,REG0,IN_PARALLEL);//2
    exit_on_warnings = OFF;
    VoC_push32(ACC0,DEFAULT);//z1
    exit_on_warnings = ON;
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);//4
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//z3 = tmp4 + tmp6;
    VoC_sw16_p(ACC1_HI,REG0,DEFAULT);//6
    VoC_multf32inc_rp(RL6,REG4,REG3,IN_PARALLEL);//z3 *= -16069;
    //////////////////////////////////////////
    VoC_multf32inc_rp(RL7,REG7,REG3,DEFAULT);//tmp7 *= 12299;
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);//z4 = tmp5 + tmp7;

    VoC_multf32inc_rp(RL6,REG5,REG3,DEFAULT);//tmp5 *= 16819;
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z3+z4
    exit_on_warnings = OFF;
    VoC_push32(RL6,DEFAULT);//z3
    exit_on_warnings = ON;
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//z2 = tmp5 + tmp6;
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);//tmp6 *= 25172;
    VoC_push32(RL7,DEFAULT);//tmp7
    VoC_multf32inc_rp(RL7,REG6,REG3,DEFAULT);//z2 *= -20995;
    VoC_multf32inc_rp(REG67,REG7,REG3,DEFAULT);//z4 *= -3196;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//tmp5 + z2
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);//tmp6 + z2

    VoC_pop32(RL7,DEFAULT);//tmp7
    VoC_push16(REG4,IN_PARALLEL);//(z3 + z4)

    VoC_lw32_sd(REG45,1,DEFAULT);//z1
    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//tmp7 + z1
    VoC_pop16(REG4,IN_PARALLEL);

    VoC_multf32_rp(REG45,REG4,REG3,DEFAULT);//z5 = (z3 + z4) * 9633;
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_push32(ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//z4 += z5;
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);//z3
    VoC_add32_rr(REG45,ACC1,REG45,DEFAULT);//z3 += z5;
    VoC_pop32(ACC1,IN_PARALLEL);


    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);//dataptr[7] = (tmp4 + z1 + z3) >> 11;//->acc0
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//dataptr[5] = (tmp5 + z2 + z4) >> 11;//rl6
    VoC_add32_rr(ACC1,ACC1,REG45,DEFAULT);//dataptr[3] = (tmp6 + z2 + z3) >> 11;//acc1
    VoC_add32_rr(RL7,RL7,REG67,IN_PARALLEL);//dataptr[1] = (tmp7 + z1 + z4) >> 11;//rl7


    VoC_pop32(REG45,DEFAULT);//z3
    VoC_lw16i_short(REG6,-5,IN_PARALLEL);

    VoC_pop32(REG45,DEFAULT);//z1
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//dataprt[1] addr


    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);

    VoC_sw16inc_p(RL7_HI,REG0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,IN_PARALLEL);
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//r0
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC0_HI,REG0,DEFAULT);
    exit_on_warnings = ON;
    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);
    VoC_endloop0

    /////part 2//////////////////////////////
    VoC_lw32_sd(REG01,0,DEFAULT);//dataptr = data;
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);//dataptr[32] addr
    VoC_lw16i_short(INC0,8,IN_PARALLEL);

    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_lw16i_short(REG5,-7,IN_PARALLEL);
    /////loop of move begin///////////////
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_loop_i(1,8);
    VoC_loop_i_short(2,DEFAULT);
    VoC_movreg16(REG4,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings = ON;
    VoC_endloop0
    VoC_add16_rr(REG0,REG4,REG5,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop1

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    //VoC_loop_i_short(8,DEFAULT);//for (i = 8; i; i--)
    VoC_loop_i(0,8)
    //VoC_startloop0

    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_short(FORMAT32,-16+15,IN_PARALLEL);
    VoC_sub16_pp(REG7,REG0,REG1,DEFAULT);//tmp7 = dataptr[0] - dataptr[56];
    VoC_add16inc_pp(REG4,REG0,REG1,IN_PARALLEL);//tmp0 = dataptr[0] + dataptr[56];
    VoC_sub16_pp(REG6,REG0,REG1,DEFAULT);//tmp6 = dataptr[8] - dataptr[48];
    VoC_add16inc_pp(REG5,REG0,REG1,DEFAULT);//tmp1 = dataptr[8] + dataptr[48];
    VoC_push32(REG67,DEFAULT);//<-tmp6 tmp7
    VoC_push32(REG45,DEFAULT);//<-tmp0 tmp1
    VoC_sub16_pp(REG5,REG0,REG1,DEFAULT);//tmp5 = dataptr[16] - dataptr[40];
    VoC_add16inc_pp(REG6,REG0,REG1,IN_PARALLEL);//tmp2 = dataptr[16] + dataptr[40];
    VoC_sub16_pp(REG4,REG0,REG1,DEFAULT);//tmp4 = dataptr[24] - dataptr[32];
    VoC_add16inc_pp(REG7,REG0,REG1,IN_PARALLEL);//tmp3 = dataptr[24] + dataptr[32];
    VoC_pop32(ACC0,DEFAULT);//->tmp0 tmp1
    VoC_push32(REG45,DEFAULT);//<-tmp4 tmp5
    VoC_movreg32(REG45,ACC0,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//tmp10 = tmp0 + tmp3;
    VoC_sub16_rr(REG7,REG4,REG7,IN_PARALLEL);//tmp13 = tmp0 - tmp3;
    VoC_add16_rr(REG5,REG6,REG5,DEFAULT);//tmp11 = tmp1 + tmp2;
    VoC_sub16_rr(REG6,REG5,REG6,IN_PARALLEL);//tmp12 = tmp1 - tmp2;

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);//dataptr[0] = (tmp10 + tmp11) >> 2;
    VoC_sub16_rr(REG5,REG4,REG5,IN_PARALLEL);//dataptr[32] = (tmp10 - tmp11) >> 2;

    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);

    //z1 = (tmp12 + tmp13) * FIX_0_541196100;
    VoC_movreg32(RL6,REG45,DEFAULT);//dataptr[0] dataptr[32
    VoC_add16_rr(REG4,REG6,REG7,IN_PARALLEL);//tmp12 + tmp13

    VoC_multf32inc_rp(ACC0,REG4,REG3,DEFAULT);
    VoC_lw16i_short(INC0,16,IN_PARALLEL);
    /////////////////////////////////////////////
    VoC_mac32inc_rp(REG7,REG3,DEFAULT);//dataptr[16] = (z1 + (tmp13 * FIX_0_765366865)) >> 15;
    exit_on_warnings = OFF;
    VoC_movreg32(ACC1,ACC0,DEFAULT);
    VoC_mac32inc_rp(REG6,REG3,IN_PARALLEL);//dataptr[48] = (z1 + (tmp12 * -FIX_1_847759065)) >> 15;
    exit_on_warnings = ON;

    VoC_lw32_sd(REG45,0,DEFAULT);//tmp4 tmp5
    VoC_lw16_sd(REG0,0,IN_PARALLEL);
    VoC_lw32_sd(REG67,1,DEFAULT);//tmp6 tmp7
    //VoC_shr32_ri(ACC1,15,IN_PARALLEL);
    ///store some elements in to the ram//  op
    VoC_sw16inc_p(RL6_LO,REG0,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z1 = tmp4 + tmp7;
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);//z1 *= -FIX_0_899976223;
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);
    VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    ///////////////////////////////////////

    VoC_mac32inc_rp(REG4,REG3,DEFAULT);//tmp4 *= FIX_0_298631336;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//z3 = tmp4 + tmp6;

    VoC_multf32inc_rp(RL6,REG4,REG3,DEFAULT);//z3 *= -16069;
    exit_on_warnings = OFF;
    VoC_push32(ACC0,DEFAULT);//z1
    exit_on_warnings = ON;
    VoC_multf32inc_rp(RL7,REG7,REG3,DEFAULT);//tmp7 *= 12299;
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);//z4 = tmp5 + tmp7;
    VoC_push32(RL6,DEFAULT);//z3
    VoC_multf32inc_rp(RL6,REG5,REG3,DEFAULT);//tmp5 *= 16819;
    VoC_add16_rr(REG4,REG4,REG7,IN_PARALLEL);//z3+z4

    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);//z2 = tmp5 + tmp6;
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);//tmp6 *= 25172;
    VoC_push32(RL7,DEFAULT);//tmp7
    VoC_multf32inc_rp(RL7,REG6,REG3,DEFAULT);//z2 *= -20995;
    VoC_multf32inc_rp(REG67,REG7,REG3,DEFAULT);//z4 *= -3196;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);//tmp5 + z2
    VoC_add32_rr(ACC1,ACC1,RL7,IN_PARALLEL);//tmp6 + z2

    VoC_pop32(RL7,DEFAULT);//tmp7
    VoC_push16(REG4,IN_PARALLEL);//(z3 + z4)

    VoC_lw32_sd(REG45,1,DEFAULT);//z1
    VoC_add32_rr(RL7,REG45,RL7,DEFAULT);//tmp7 + z1
    VoC_pop16(REG4,IN_PARALLEL);

    VoC_multf32_rp(REG45,REG4,REG3,DEFAULT);//z5 = (z3 + z4) * 9633;
    VoC_push32(ACC1,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);
    VoC_add32_rr(REG67,REG67,REG45,DEFAULT);//z4 += z5;
    VoC_lw32_sd(ACC1,1,IN_PARALLEL);//z3
    VoC_add32_rr(REG45,ACC1,REG45,DEFAULT);//z3 += z5;
    VoC_pop32(ACC1,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);//dataptr[56] = (tmp4 + z1 + z3) >> 15;//->acc0
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//dataptr[40] = (tmp5 + z2 + z4) >> 15;//rl6
    VoC_add32_rr(ACC1,ACC1,REG45,DEFAULT);//dataptr[24] = (tmp6 + z2 + z3) >> 15//acc1
    VoC_add32_rr(RL7,RL7,REG67,IN_PARALLEL);//dataptr[8] = (tmp7 + z1 + z4) >> 15//rl7

    VoC_lw16i_short(REG6,8,DEFAULT);//dataptr[8]
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);//dataptr[8] addr
    VoC_add16_rr(REG7,REG0,REG7,IN_PARALLEL);//prepare r0 for the next loop

    VoC_lw16i_set_inc(REG3,JPEGENC_MYDCT_ADDR,1);

    VoC_sw16inc_p(RL7_HI,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);//z1
    VoC_sw16inc_p(ACC1_HI,REG0,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);//z3
    VoC_sw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_movreg16(REG0,REG7,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw16_p(ACC0_HI,REG0,DEFAULT);
    VoC_lw16i_short(INC0,8,IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_endloop0

    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(INC0,-2,IN_PARALLEL);


    VoC_return;
}

/* format_number:
 *  Computes the category and bits of a given number.
 */
void format_number(void)
{

    // REGS[4].reg = (short)num;
    //input:r4
    //output: r0:Addr  r4:cat   r5:bits
    //used buf: r3 r4 r5 r6 r7
    //VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_push16(REG3,DEFAULT);//num_zeroes
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_movreg16(REG5,REG4,DEFAULT);//mask = num >> 31;
    VoC_shr16_ri(REG4,15,IN_PARALLEL);//r4:mask  r5:num

    VoC_xor16_rr(REG4,REG5,DEFAULT);//abs_num = (num ^ mask) - mask;
    VoC_movreg16(REG6,REG4,IN_PARALLEL);//r6:mask  r4:(num ^ mask)

    VoC_shru16_ri(REG6,15,DEFAULT);//num_sign = mask&0x1;
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);//abs_num

    VoC_loop_i_short(16,DEFAULT);//for (cat = 0; abs_num; cat++)
    VoC_lw16i_short(REG3,0,IN_PARALLEL);
    VoC_startloop0;
    VoC_be16_rd(LABLE_FORMAT_NUM_CAT,REG4,CONST_0_ADDR);//abs_num >>= 1;
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);//r3:-cat//*category = cat;
    VoC_endloop0;
LABLE_FORMAT_NUM_CAT://r4==0

    VoC_shr16_rr(REG6,REG3,DEFAULT);//(num_sign <<cat)
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//num-num_sign

    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);//*bits = num +(num_sign <<cat)-num_sign;
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);//cab

    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_sub16_rr(REG0,REG0,REG3,DEFAULT);//r0+2*cat
    VoC_pop16(REG3,IN_PARALLEL);//REGS[3].reg = (short)num_zeroes;

    VoC_mult16_rd(REG3,REG3,CONST_20_ADDR);//num * 20

    //*bits = REGS[5].reg;
    //*category = REGS[4].reg;
    VoC_return;
}

/* put_bits:
 *   Writes some bits to the output stream.
 */
void COOLSAND_put_bits(void)
{
    //input:value:r7  numbits:r6
    //used buf:r3 r4 r5 r6 r7
    //output: r45->r67

    VoC_push16(REG3,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lw32_d(REG45,GLOBAL_CURRBITS_ADDR);


    VoC_bnlt16_rd(LABLE_PUTBITS_WILE_BEGIN1,REG6,GLOBAL_CURRBITS_ADDR);
    VoC_jump(LABLE_PUTBITS_WILE_END);
LABLE_PUTBITS_WILE_BEGIN1:
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);
LABLE_PUTBITS_WILE_BEGIN:
    //while(num_bits >= current_bit)
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//num_bits -= current_bit;

    VoC_shru16_rr(REG7,REG6,DEFAULT);//current_byte |= (value >> num_bits);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);
    exit_on_warnings = ON;

    VoC_and16_ri(REG5,0x00ff);

    VoC_bne16_rd(LABLE_PUTBITS_0XFF,REG5,CONST_0xff_ADDR);//if ((current_byte&0xff) == 0xff)
    //*chunkptr++ = 0;
    VoC_bez16_d(LABLE_PUTBITS_WRITE_short_1,GLOBAL_SHORT_1_ADDR);//=1
    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_or16_rd(REG5,GLOBAL_pp_ADDR);//the written byte is ff**
    VoC_sw16_d(REG6,GLOBAL_pp_ADDR);//0 in the pp addr
LABLE_PUTBITS_WRITE_short_1://=0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);//sw
    VoC_jump(LABLE_PUTBITS_WRITE_1024_begin);

LABLE_PUTBITS_0XFF://*chunkptr++ = current_byte;
    VoC_bez16_d(LABLE_PUTBITS_WRITE_ORNOT,GLOBAL_SHORT_1_ADDR);//=1
    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_or16_rd(REG5,GLOBAL_pp_ADDR);
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);//sw
    VoC_jump(LABLE_PUTBITS_WRITE_1024_begin);
LABLE_PUTBITS_WRITE_ORNOT://=0
    VoC_sw16_d(REG5,GLOBAL_pp_ADDR);
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);


LABLE_PUTBITS_WRITE_1024_begin:
    VoC_blt16_rd(LABLE_PUTBITS_WRITE_1024,REG3,CONST_512_ADDR);//if(chunkptr == chunk_end)

    VoC_push16(REG2,DEFAULT);
    ////////dmai/////////////////////
    VoC_lw16i(REG2,256);//size[int]
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//ADDR-[int]
    VoC_jal(DMAI_WRITE);//return r2=1024
    VoC_pop16(RA,DEFAULT);
    //////dmae//////////////////////

    //compute  codelength//
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_add32_rd(REG23,REG23,Output_output_len_addr);//byte

    VoC_lw16i_short(REG3,0,DEFAULT);//recover the pointor
    VoC_pop16(REG2,IN_PARALLEL);

    exit_on_warnings = OFF;
    VoC_sw32_d(REG23,Output_output_len_addr);
    exit_on_warnings = ON;
LABLE_PUTBITS_WRITE_1024:


    VoC_lw16i_short(REG4,8,DEFAULT);//current_bit = 8;
    VoC_pop32(REG67,DEFAULT);//r7:value  r6:num
    VoC_lw16i_short(REG5,0,IN_PARALLEL);//current_byte = 0;
    VoC_bngt16_rr(LABLE_PUTBITS_WILE_BEGIN,REG4,REG6);//while(num_bits >= current_bit)


    VoC_sw16_d(REG3,GLOBAL_CURR_POS_ADDR);

LABLE_PUTBITS_WILE_END:
    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//current_bit -= num_bits;
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);

    VoC_shru16_rr(REG7,REG6,DEFAULT);//current_byte |= (value << current_bit);
    VoC_or16_rr(REG5,REG7,DEFAULT);

    VoC_pop32(REG67,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    VoC_sw32_d(REG45,GLOBAL_CURRBITS_ADDR);

    VoC_return;//return 0;
}

void COOLSNAD_flush_bits(void)
{
    //used buf:r3 r4 r5 r6 r7
    VoC_lw32_d(REG45,GLOBAL_CURRBITS_ADDR);
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);//current_bit--;
    ////////////////////////////////////////////////////////////////////
    VoC_bgt16_rd(LABLE_FLUSH_WHILE_BEGIN,REG4,CONST_NEG7_ADDR);
    VoC_jump(LABLE_FLUSH_WHILE_END2);
LABLE_FLUSH_WHILE_BEGIN://  while (current_bit < 7)  -current_bit > -7
    VoC_shru16_rr(REG6,REG4,DEFAULT);//1 << current_bit
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//current_bit--;

    VoC_or16_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_bngt16_rd(LABLE_FLUSH_WHILE_BEGIN,REG4,CONST_0_ADDR);//if (current_bit < 0)


    //if (current_byte == 0xff) _jpeg_putc(0);
    VoC_bne16_rd(LABLE_FLUSH_0XFF,REG5,CONST_0xff_ADDR);//if ((current_byte&0xff) == 0xff)
    VoC_bez16_d(LABLE_FLUSH_WRITE_short_1,GLOBAL_SHORT_1_ADDR);//=1
    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_or16_rd(REG5,GLOBAL_pp_ADDR);
    VoC_sw16_d(REG6,GLOBAL_pp_ADDR);
LABLE_FLUSH_WRITE_short_1://=0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_jump(LABLE_FLUSH_WRITE_1024_begin);


LABLE_FLUSH_0XFF:
    VoC_bez16_d(LABLE_FLUSH_WRITE_ORNOT,GLOBAL_SHORT_1_ADDR);//=1
    VoC_shru16_ri(REG5,-8,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_or16_rd(REG5,GLOBAL_pp_ADDR);
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);//0
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_jump(LABLE_FLUSH_WRITE_ORNOT_END);
LABLE_FLUSH_WRITE_ORNOT://=0
    VoC_sw16_d(REG5,GLOBAL_pp_ADDR);
    VoC_sw16_d(REG6,GLOBAL_SHORT_1_ADDR);//1
LABLE_FLUSH_WRITE_ORNOT_END:


LABLE_FLUSH_WRITE_1024_begin:
    VoC_bne16_rd(LABLE_CHANGE_10,REG3,CONST_512_ADDR);//if(chunkptr == chunk_end)
    VoC_push16(REG2,DEFAULT);
    //////dmai///////////////////////
    VoC_lw16i(REG2,256);//size[int]
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);//ADDR-[int]
    VoC_jal(DMAI_WRITE);
    VoC_pop16(RA,DEFAULT);
    //////dmae//////////////////////
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_add32_rd(REG23,REG23,Output_output_len_addr);//byte

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    exit_on_warnings = OFF;
    VoC_sw32_d(REG23,Output_output_len_addr);
    exit_on_warnings = ON;
LABLE_CHANGE_10:
    VoC_sw16_d(REG3,GLOBAL_CURR_POS_ADDR);

LABLE_FLUSH_WHILE_END2:
    //flush  ff d9

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG6,0xd9ff);
    VoC_lw16d_set_inc(REG3,GLOBAL_CURR_POS_ADDR,1);
    VoC_be16_rd(LABLE_WRITE_ODD,REG4,GLOBAL_SHORT_1_ADDR);
    VoC_lw16_d(REG5,GLOBAL_pp_ADDR);
    VoC_or16_ri(REG5,0xff00);
    VoC_lw16i(REG6,0xd9);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_jump(LABLE_WRITE_FILE);
LABLE_WRITE_ODD:
    VoC_sw16inc_p(REG6,REG3,DEFAULT);//_jpeg_putw(CHUNK_EOI);
LABLE_WRITE_FILE:

    /////VoC_jal(DMAI_WRITE);////////////////////////
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_shru16_ri(REG3,-1,IN_PARALLEL);//byte  size

    VoC_movreg16(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

    VoC_lw16i_short(REG7,4,DEFAULT);//[kind]
    VoC_lw16i_short(REG4,15,IN_PARALLEL);
    VoC_add32_rd(ACC0,ACC0,Output_output_len_addr);//r6 is the total byte of encode data
    VoC_sw16_d(REG7,Output_mode_addr);
    VoC_sw32_d(ACC0,Output_output_len_addr);

    VoC_add16_rr(REG4,REG3,REG4,DEFAULT);//VoC_lw16i(REG4,256);
    VoC_shr16_ri(REG4,4,DEFAULT);
    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_lw32_d(ACC1,Input_outStreamBufAddr_addr);//BYTE

    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_return;

}


/////////////////////////////////////////////////////////
void  DMAI_WRITE(void)
{

    VoC_lw32_d(ACC1,Input_outStreamBufAddr_addr);//BYTE


    VoC_sw32_d(REG23,CFG_DMA_SIZE);
    VoC_shru16_ri(REG2,-2,DEFAULT);//->byte

    VoC_add32_rr(ACC1,ACC1,REG23,DEFAULT);//VoC_lw16i(ACC1_HI,0x4000
    exit_on_warnings = OFF;
    VoC_sw32_d(ACC1,CFG_DMA_EADDR);
    exit_on_warnings = ON;
    VoC_sw32_d(ACC1,Input_outStreamBufAddr_addr);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_return;

}



/////////////////////////////////

void CII_AMR_Encode(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_or16_rd(REG2,GLOBAL_RESET_FLAG_ADDR);   //reset_flag
    VoC_bez16_r(NO_RESET_ENC,REG2)
    //reset_enc
    VoC_jal(CII_encoder_reset);

    /*get new speech from in buffer*/

NO_RESET_ENC:
    VoC_lw16i_short(REG6,0XFFF8,DEFAULT);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);

    VoC_movreg16(REG7,REG6,DEFAULT);

    VoC_lw16i_set_inc(REG0,INPUT_SPEECH_BUFFER_ADDR,2);
    VoC_loop_i(0,80)

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_and32_rr(RL6,REG67,DEFAULT);
    VoC_NOP();

    VoC_sw32inc_p(RL6,REG1,DEFAULT);
    VoC_endloop0

    /* Check whether this frame is an encoder homing frame */

    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);
    VoC_jal(CII_encoder_homing_frame_test);
    VoC_sw16_d(REG4,GLOBAL_RESET_FLAG_ADDR);


    // preprocess filter, copy(new_speech, st->new_speech, L_FRAME);
    VoC_jal(CII_Pre_Process);


    /*------------------------------------------------------------------------*
     *  - Perform LPC analysis:                                               *
     *       * autocorrelation + lag windowing                                *
     *       * Levinson-durbin algorithm to find a[]                          *
     *       * convert a[] to lsp[]                                           *
     *       * quantize and code the LSPs                                     *
     *       * find the interpolated LSPs and convert to a[] for all          *
     *         subframes (both quantized and unquantized)                     *
     *------------------------------------------------------------------------*/

    /* LP analysis */

    VoC_lw16i(REG7,COD_AMR_ANA_ADDR);

    VoC_jal(CII_COD_AMR_122);

//cod_amr_end:


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}



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
    VoC_bnlt32_rd(G_PITCH_NORM_L_3_EXIT,ACC0,CONST_0x40000000L_ADDR)
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
void CII_encoder_homing_frame_test(void)
{
    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_loop_i(0,80)
    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_bne32_rd(CII_encoder_homing_frame_test_01,REG67,CONST_0x00080008_ADDR)
    VoC_endloop0

    VoC_lw16i_short(REG4,1,DEFAULT);

CII_encoder_homing_frame_test_01:

    VoC_return

}

/*****************************************************************************
 * Function CII_Copy_M()        REG0->REG1                                          *
  Input  registers: REG0(incr 2),REG1(incr 2)
  Used   registers:  REG0,REG1,ACC0                            *
 ******************************************************************************/

void CII_Copy_M(void)
{
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_loop_i_short(5,DEFAULT);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return;
}

/*
INPUT:
REG0 and REG1 :sourec and dest address  INC 2
REG2   :scale value (shr)
REG3   :(number of Word16s to be scaled)/4

USERD   :REG45,REG67
*/
void CII_scale(void)
{
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_loop_r_short(REG3,DEFAULT)
    VoC_startloop0
    VoC_shr16_rr(REG4,REG2,DEFAULT);
    VoC_shr16_rr(REG5,REG2,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_return
}



void CII_copy_xy(void)
{
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);
    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_return

}

/*
INPUT:
REG0: source address INC 2
REG1: dest   address INC 2
REG6: (number of Long Words copyed)/2 or (number of Word16 copyed)/4

USED:   ACC0,ACC1
*/


/*
need
r3:dest addr
r2:i0
    */
//used regigister  REG0 ,1,2,3 ,4,

void CII_SEARCH_COPY(void)
{
// compute first address i0*(i0-1)/2 +i0
    VoC_mult16_rr(REG0, REG2, REG2,DEFAULT);

    VoC_lw16i_short(INC2,1,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_movreg16(REG1, REG2,IN_PARALLEL);

    VoC_sub16_rr(REG0, REG0, REG2,DEFAULT);
    VoC_shr16_ri(REG0,1,DEFAULT);

    VoC_add32_rd(REG01, REG01, C1035PF_RR_SIDE_ADDR_ADDR);

    // copy abscissa number loop number is i0
    VoC_bez16_r(SEARCH_COPY_label_100, REG2)
    VoC_lw16inc_p(REG4, REG0,DEFAULT);
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4, REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_sub16_rd(REG0, REG0,CONST_2_ADDR);  // read a useless word
SEARCH_COPY_label_100:

    // copy ordinate number loop number is 39-i0
    VoC_lw16_p(REG1, REG1,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    VoC_sub16_dr(REG1, CONST_39_ADDR, REG2);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG1, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_bez16_r(SEARCH_COPY_label2_200, REG1)
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    VoC_loop_r_short(REG1,DEFAULT)
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;
SEARCH_COPY_label2_200:
    VoC_return
}



void CII_encoder_reset (void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw32z(ACC1,DEFAULT);

    VoC_lw16i_set_inc(REG0,STATIC_L_SACF_ADDR,2);
    VoC_loop_i(1,179)
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(RL6,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_endloop1;


    VoC_lw16i_set_inc(REG0,AMR_GLOBAL_VARS_Y_START_ADDR,2);
    VoC_loop_i(1,112)
    VoC_sw32inc_p(RL7,REG0,DEFAULT);
    VoC_endloop1;


    VoC_jal(CII_RESET_AMR_CONSTANT);


    VoC_lw16i_set_inc(REG0,AMR_ENC_RESET_TABLE_ADDR,1); // reset address
    VoC_lw16i(REG6,AMR_EFR_RESET_Offset_X );

    VoC_lw16inc_p(REG2,REG0,DEFAULT);//ram addr
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_loop_i(1,24)


    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,IN_PARALLEL);//loop time


    VoC_loop_r_short(REG5,DEFAULT)
    VoC_lw16inc_p(REG3,REG0,IN_PARALLEL);//rom value
    VoC_startloop0

    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG3,REG2,DEFAULT);
    exit_on_warnings=ON;
    // fprintf(fp,"0x%0x ,",REGS[7].reg);
    // i++;
    VoC_endloop0
    // fprintf(fp,"\n");
    VoC_movreg16(REG2,REG3,DEFAULT);

    VoC_endloop1




//  VoC_lw16i_short(REG4,8,DEFAULT);
//  VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP()
//      VoC_sw16_d(REG4,STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR);

//  VoC_sw16_d(REG6,STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR);

    VoC_return;
}


/***********************************************
 Function : CII_dtx_com_s2
 Input: REG0(incr=2)    :&lsf[](&lsp[])
    REG1(incr=2)    :&st->lsf_hist[](&st->lsp_hist[])
        REG7        :st->lsf_hist_ptr(st->hist_ptr*8)
    REG2(incr=2)    :&frame[](&speech[])
 Output:REG4        :log_en
 Note: dtx_dec_activity_update and dtx_buffer share the same code
  Optimized by Kenneth  22/09/2004
***********************************************/

void CII_dtx_com_s2(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_jal(CII_Copy_M);                //Copy(lsf, &st->lsf_hist[st->lsf_hist_ptr], M);

    VoC_lw32z(ACC0,DEFAULT);            //L_frame_en = 0;
    VoC_loop_i(0,80);               //for (i=0; i < L_FRAME; i++)
    VoC_bimac32inc_pp(REG2,REG2);       //{   L_frame_en = L_mac(L_frame_en, frame[i], frame[i]);
    VoC_endloop0                    //}
    VoC_lw16i(REG6,8521);
//      VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_jal(CII_Log2);              //Log2(L_frame_en, &log_en_e, &log_en_m);
    VoC_shr16_ri(REG4,-10,DEFAULT);         //log_en = shl(log_en_e, 10);
    VoC_shr16_ri(REG5,5,DEFAULT);       //log_en = add(log_en, shr(log_en_m, 15-10));
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);       // log_en = sub(log_en, 8521);
    VoC_return;
}


/****************************************************************************
 *Div_32
 *INPUT: REG45: L_nom;  REG7 :denom_hi; REG6:denom_lo, REG3 = 1
 *output:ACC0:  division result
 *       ACC1:  L_nom
 *used  :all
 *unchanged : REG3
 ***************************************************************************/

void CII_DIV_32(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_movreg32(ACC1,REG45,DEFAULT);

    VoC_bnltz32_r(DIV_32_L_ABS_IN,REG45)
    VoC_sub32_dr(REG45,CONST_WORD32_0_ADDR,REG45);   //    t1 = L_abs (t0);
DIV_32_L_ABS_IN:
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16i(REG0,0x3fff);
    VoC_movreg16(REG1, REG7,DEFAULT);


    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);
    // REG2 approx
    VoC_multf16_rr(REG0, REG6, REG2,DEFAULT);
    VoC_multf32_rr(ACC0, REG7, REG2,DEFAULT);

    VoC_mac32_rr(REG0, REG3,DEFAULT);
    VoC_shru16_ri(REG4, 1,DEFAULT);
    VoC_sub32_dr(REG67, CONST_0x7FFFFFFF_ADDR, ACC0);
    VoC_shru16_ri(REG6, 1,DEFAULT);

    VoC_multf16_rr(REG0, REG6, REG2,DEFAULT);
    VoC_multf32_rr(ACC0, REG7, REG2,DEFAULT);
    VoC_mac32_rr(REG0, REG3,DEFAULT);
    VoC_NOP();
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shru16_ri(REG6, 1,DEFAULT);

    VoC_multf32_rr(ACC0, REG5, REG7,DEFAULT);
    VoC_multf16_rr(REG0, REG5, REG6,IN_PARALLEL);

    VoC_multf16_rr(REG6, REG4, REG7,DEFAULT);
    VoC_mac32_rr(REG0, REG3,DEFAULT);


    VoC_mac32_rr(REG6, REG3,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_shr32_ri(ACC0,-2,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);

    VoC_bngtz32_r(DIV_32_L_ABS_OUT,ACC1)                    //   if (t0 > 0)
    VoC_sub32_dr(REG67,CONST_WORD32_0_ADDR,REG67);  //    t2 = L_negate (t2);         /* K =-t0/Alpha                */
DIV_32_L_ABS_OUT:

    VoC_return
}



/****************************************************************
    *1/sqrt
    *INPUT:         REG01
    *OUTPUT:        REG01
    *USED:          REG2,REG6,ACC0,LOOP0
    ******************************************************************/

void CII_Inv_sqrt(void)
{
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bgt32_rd(INV_SQRT101,REG01,CONST_WORD32_0_ADDR)
    VoC_lw16i_short(REG0,(Word16)0xffff,DEFAULT);
    VoC_lw16i(REG1,0x3fff);

    VoC_return;
INV_SQRT101:
    /* REG2 for exp = norm_l (L_x);*/
    VoC_lw16i_set_inc(REG2,0,1);
    VoC_loop_i_short(31,DEFAULT)
    VoC_startloop0
    VoC_bnlt32_rd(NORM_L105,REG01,CONST_0x40000000_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_shr32_ri(REG01,-1,IN_PARALLEL);
    VoC_endloop0

NORM_L105:;
    /*Normalization of L_x is done in norm_l*/

    VoC_sub16_dr(REG2,CONST_30_ADDR,REG2);

    /*If (exponent=30-exponent) is even then shift right once. */

    VoC_and16_rr(REG6,REG2,DEFAULT);
    VoC_shr16_ri(REG2,1,IN_PARALLEL);
    VoC_bnez16_r(INV_SQRT102,REG6)
    VoC_shr32_ri(REG01,1,DEFAULT);
INV_SQRT102:

    /*exponent = exponent/2  +1*/

    /* i = bit25-b31 of L_x; */
    VoC_shr32_ri(REG01,9,DEFAULT);          //REG1 for i

    /*i -=16*/
    VoC_sub16_rd(REG6,REG1,CONST_16_ADDR);
    /*a = bit10-b24*/
    VoC_inc_p(REG2,DEFAULT);
    VoC_shr32_ri(REG01,1,IN_PARALLEL);


    /*L_y = table[i]<<16 - (table[i] - table[i+1]) * a * 2*/
    VoC_lw16i_set_inc(REG1,TABLE_INV_SQRT_ADDR,1);
    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(ACC0_LO,0,IN_PARALLEL);
    VoC_and16_ri(REG0,0X7FFF);              //REG0 for a

    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_movreg16(ACC0_HI,REG6,DEFAULT);         //ACC0 for L_y
    VoC_sub16_rp(REG1,REG6,REG1,IN_PARALLEL);
    VoC_msu32_rr(REG1,REG0,DEFAULT);
    VoC_NOP();
    /*L_y >>= exponent*/
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_shr32_rr(REG01,REG2,DEFAULT);

    //--------------------------------------------------------------------
    VoC_return
}


void CII_Lag_window (void)
{

    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR+1,1);          //&r_l[1]
    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR+1,1);         //&r_h[1]
    VoC_lw16i_set_inc(REG2,TABLE_LAG_L_ADDR,1);             //&lag_l
    VoC_lw16i_set_inc(REG3,TABLE_LAG_H_ADDR,1);             //&lag_h

    VoC_lw32_d(REG45,CONST_0x00010001_ADDR);

    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_multf16_pp(REG6,REG1,REG2,DEFAULT);
    VoC_multf16_pp(REG7,REG0,REG3,DEFAULT);
    VoC_multf32_pp(ACC0,REG1,REG3,DEFAULT);
    VoC_bimac32_rr(REG45,REG67);
    VoC_add32_rr(REG23,REG23,REG45,DEFAULT);
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_shru16_ri(REG6,1,DEFAULT);
//      VoC_and16_ri(REG6,0X7FFF);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);

    VoC_endloop0

    VoC_return;

}



void CII_Levinson (void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR,1);
    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR,1);

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_push32(REG01,DEFAULT);

    // count=0;
    VoC_lw16inc_p(REG7,REG1,DEFAULT);      // Rh[0]
    VoC_lw16inc_p(REG6,REG0,DEFAULT);      // Rl[0]

    VoC_multf32_rp(REG45,REG3,REG0,DEFAULT);    // L_deposit_lo(Rl[1]) << 1
    VoC_push32(REG67,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);              // L_deposit_hi(Rh[1])
    /*************************
     *FUNC  :     Div_32
     *INPUT :     REG45 <- L_nom;
     *            REG67 <- L_denom;
     *OUTPUT:     REG67 <- L_res
     *            ACC1  <- L_nom
     *            REG3  <- 1
     *USED      : all
     *************************/

    VoC_jal(CII_DIV_32);
    VoC_movreg32(REG45,REG67,DEFAULT);
    VoC_lw32_d(REG23,CONST_0x7FFFFFFF_ADDR);
    VoC_add32_rd(ACC0,REG67,CONST_0x00008000L_ADDR);


    VoC_shr16_ri(REG6,1,DEFAULT);

    VoC_and16_rr(REG6,REG3,DEFAULT);             //   L_Extract (t0, &Kh, &Kl);           /* K in DPF         */
    VoC_shr32_ri(REG45,4,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);




    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG6,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16_d(ACC0_HI,LPC_RC_ADDR);
    exit_on_warnings=ON;
    VoC_mac32_rd(REG0,CONST_1_ADDR);              //  REG3      1
    VoC_mac32_rd(REG0,CONST_1_ADDR);           // t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */

    VoC_sw32_d(REG45,LEVINSON_AL_ADDR+2);    //  L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

    VoC_bnltz32_r(LEVINSON_L_ABS_2,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);
LEVINSON_L_ABS_2:

    VoC_sub32_rr(REG45,REG23,ACC0,DEFAULT); // REG23 = 0X7FFFFFFF
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    // REG2 = -1

    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG4,IN_PARALLEL);
    VoC_multf16_rr(REG0,REG6,REG5,DEFAULT);
    exit_on_warnings = OFF;
    VoC_msu32_rr(REG0,REG2,DEFAULT);
    exit_on_warnings = ON;
    VoC_msu32_rr(REG0,REG2,DEFAULT);

    /**************************/
    // func  : CII_NORM_L_ACC0
    // input : ACC0
    // output: REG1
    // used  : ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG3,DEFAULT);    //REG3 = 0x7FFF

    VoC_push16(REG1,IN_PARALLEL);           // alp_exp

    VoC_lw16i_short(REG6,2,DEFAULT);    //i = 2
    VoC_push32(REG45,DEFAULT);
    VoC_push16(REG6,DEFAULT);

    //stack16  RA    alp_exp    i
    //stack32  Rh[0]&Rl[0],     alp_h&alp_l

    VoC_loop_i(1,9)

    VoC_lw32_sd(REG01,1,DEFAULT);   // &Rh[0], &Rl[0],
    VoC_lw16i_short(REG5,1,DEFAULT);

    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR-2+1,-1); // point to hi

    VoC_lw16_sd(REG6,0,DEFAULT);    // i
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG6,REG4,DEFAULT);//number of loops
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);  // 2*i

    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);      ////&Ah[i-1]

    // VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add32_rr(REG01,REG01,REG45,IN_PARALLEL);//  Rh[1], Rl[1],

    VoC_loop_r_short(REG7,DEFAULT)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32_pp(REG1,REG2,DEFAULT);
    VoC_multf16inc_pp(REG6,REG0,REG2,DEFAULT);
    VoC_multf16inc_pp(REG7,REG1,REG2,DEFAULT);
    VoC_NOP();
    VoC_bimac32_rr(REG67,REG45);
    VoC_endloop0

    VoC_lw32_sd(REG01,1,DEFAULT);       // Rh[0], Rl[0]
    VoC_lw16_sd(REG6,0,IN_PARALLEL);    // i

    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);       //Rh[i], Rl[i],

    VoC_shr32_ri(ACC0,-4,DEFAULT);

    VoC_multf32_rp(REG45,REG4,REG0,DEFAULT); // REG4 = 1
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_lw16_p(REG5,REG1,DEFAULT);//  t1 = L_Comp (Rh[i], Rl[i]);

    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);  //   t0 = L_add (t0, t1);            /* add R[i]        */


    /*************************
     *FUNC  :     Div_32
     *INPUT :     REG45 <- L_nom;
     *            REG67 <- L_denom;
     *OUTPUT:     REG67 <- L_res
     *            ACC1  <- L_nom
     *            REG3  <- 1
     *USED      : all
     *************************/
    VoC_jal(CII_DIV_32);

    VoC_lw16_sd(REG2,1,DEFAULT); // alp_exp
    VoC_sub16_dr(REG2,CONST_0_ADDR,REG2);

    VoC_shr32_rr(REG67,REG2,DEFAULT);
    VoC_add32_rd(ACC0,REG67,CONST_0x00008000L_ADDR);

    VoC_movreg32(ACC1, REG67,DEFAULT);   //t2
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

    //  VoC_and16_ri(REG6,0X7FFF);    //Kh   Kl

    VoC_lw16i(REG0,LPC_RC_ADDR-1);
    VoC_lw16_sd(REG4,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

    VoC_bnlt16_rd(LEVINSON_03,REG4,CONST_5_ADDR)
    VoC_sw16_p(ACC0_HI,REG0,DEFAULT);
LEVINSON_03:

    VoC_movreg16(REG5,REG7,DEFAULT);
    VoC_bnltz16_r(LEVINSON_ABS_S_1,REG5)
    VoC_sub16_dr(REG5,CONST_0_ADDR,REG5);
LEVINSON_ABS_S_1:

    VoC_lw16i(REG4,32750);

    VoC_bngt16_rr(LEVINSON_04,REG5,REG4)

    VoC_lw16i(REG0,STRUCT_LEVINSONSTATE_OLD_A_ADDR);
    VoC_lw16_d(REG1,PARA4_ADDR_ADDR);

    VoC_lw32z(RL6,DEFAULT);
    VoC_loop_i_short(11,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw32_d(RL6,LPC_RC_ADDR);
    VoC_sw32_d(RL6,LPC_RC_ADDR+2);

    VoC_jump(LEVINSON_EXIT);

LEVINSON_04:

    /*------------------------------------------*
     *  Compute new LPC coeff. -> An[i]         *
     *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
     *  An[i]= K                                *
     *------------------------------------------*/

    VoC_lw16_sd(REG4,0,DEFAULT);    // i
    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR-2+1,-1);//point to hi

    /*
      t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
      t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
      L_Extract (t0, &Anh[j], &Anl[j]);
    */
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_lw16i_set_inc(REG3,LEVINSON_ANL_ADDR,2);

    VoC_lw16i_set_inc(REG1,CONST_0x00010001_ADDR,0);//&0X00010001
    VoC_lw16i_set_inc(REG0,LEVINSON_AL_ADDR+2,1);


    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_sub16_rp(REG4,REG4,REG1,IN_PARALLEL); //--


    VoC_multf32inc_pp(RL6,REG0,REG1,DEFAULT);
    VoC_loop_r_short(REG4,DEFAULT)                   //   for (j = 1; j < i; j++)

    VoC_startloop0
    VoC_multf32_rp(ACC0,REG7,REG2,DEFAULT);
    VoC_multf16inc_rp(REG5,REG6,REG2,IN_PARALLEL);
    VoC_multf16inc_rp(REG4,REG7,REG2,DEFAULT);
    VoC_NOP();
    VoC_bimac32inc_rp(REG45,REG1);                //    t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j])
    VoC_lw16inc_p(RL6_HI,REG0,DEFAULT);
    VoC_add32_rr(REG45,RL6,ACC0,DEFAULT);  //   t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
    VoC_shru16_ri(REG4,1,DEFAULT);    //  L_Extract (t0, &Anh[j], &Anl[j]);
    VoC_multf32inc_pp(RL6,REG0,REG1,DEFAULT);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_endloop0

//LEVINSON_101:

    // REG1 = CONST_0X00010001_ADDR, independent from branch

    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG6,IN_PARALLEL);

    VoC_lw32_d(REG67,CONST_0x7FFFFFFF_ADDR);

    VoC_movreg32(REG45,ACC1,DEFAULT);
    VoC_shr32_ri(REG45,4,DEFAULT);            // t2 = L_shr (t2, 4);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG7,DEFAULT);                //   L_Extract (t2, &Anh[i], &Anl[i]);

    VoC_mac32_rp(REG0,REG1,DEFAULT);
    VoC_mac32_rp(REG0,REG1,DEFAULT);    //  t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
    VoC_sw32inc_p(REG45,REG3,DEFAULT);

    VoC_bnltz32_r(LEVINSON_L_ABS_4,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);//  t0 = L_abs (t0);                        /* Some case <0 !! */
LEVINSON_L_ABS_4:

    VoC_sub32_rr(REG23,REG67,ACC0,DEFAULT);    // t0 = L_sub ((Word32) 0x7fffffffL, t0);  /* 1 - K*K        */

    /* DPF format */
    VoC_shr16_ri(REG2,1,DEFAULT);
    VoC_and16_rr(REG2,REG7,DEFAULT);         //    L_Extract (t0, &hi, &lo);               /* DPF format      */
    VoC_lw32_sd(REG45,0,IN_PARALLEL);


    VoC_multf16_rr(REG2,REG5,REG2,DEFAULT);
    VoC_multf16_rr(REG3,REG4,REG3,DEFAULT);
    exit_on_warnings = OFF;
    VoC_multf32_rr(ACC0,REG5,REG3,DEFAULT);
    exit_on_warnings = ON;
    VoC_bimac32inc_rp(REG23,REG1);      //   t0 = Mpy_32 (alp_h, alp_l, hi, lo);

    /**************************/
    // func  : CII_NORM_L_ACC0
    // input : ACC0
    // output: REG1
    // used  : ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);
    VoC_movreg32(REG45,ACC0,DEFAULT);                   //t0

    /* DPF format    */
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG7,DEFAULT);
    VoC_lw16_sd(REG3,1,IN_PARALLEL);   //alp_exp
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_sw32_sd(REG45,0,DEFAULT);
    VoC_sw16_sd(REG3,1,DEFAULT);         //  alp_exp = add (alp_exp, j);             /* Add normalization to

    VoC_lw16i_set_inc(REG2,LEVINSON_ANL_ADDR,2);
    VoC_lw16i_set_inc(REG3,LEVINSON_AL_ADDR+2,2);

    VoC_lw16_sd(REG6,0,DEFAULT);   //i
    VoC_add16_rd(REG5,REG6,CONST_1_ADDR);

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_lw32inc_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG2,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_sd(REG5,0,DEFAULT);   //i++
    VoC_endloop1

    VoC_lw16i(REG4,4096);

    VoC_lw16d_set_inc(REG0,PARA4_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_LEVINSONSTATE_OLD_A_ADDR+1,1);//&old_A[1]
    VoC_lw16i_set_inc(REG2,LEVINSON_AL_ADDR+2,1);   //&AL[1]

    VoC_sw16inc_p(REG4,REG0,DEFAULT);                   //A[0]=4096
    VoC_multf32inc_pd(REG45,REG2,CONST_1_ADDR);
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_shr32_ri(REG45,-1,DEFAULT);
    VoC_add32_rd(REG67,REG45,CONST_0x00008000_ADDR);
    VoC_multf32inc_pd(REG45,REG2,CONST_1_ADDR);
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

LEVINSON_EXIT:

    VoC_loop_i_short(2,DEFAULT)
    VoC_startloop0
    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(ACC0,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}




/******************************************
  Function: CII_Get_lsp_pol
  Input:  &lsp[] -> REG1(incr=2)
      &f[]   -> REG2(incr=2)

   Optimized by Kenneth  09/13/2004
******************************************/

void CII_Get_lsp_pol(void)
{
    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i(ACC0_HI,0x100);
//  VoC_lw32_d(ACC0,CONST_0x1000000_ADDR);
    VoC_lw16i_short(REG0,2,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_loop_i(1,5);
    VoC_be16_rd(Get_lsp_pol_L1,REG0,CONST_2_ADDR);
    VoC_lw32_p(ACC0,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG0,REG5,IN_PARALLEL);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_sw32_p(ACC0,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_sub16_rr(REG6,REG2,REG5,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG7,DEFAULT);   // here reg7=4
    VoC_lw32_p(ACC0,REG2,IN_PARALLEL);
    VoC_lw32_p(REG67,REG6,DEFAULT);
    VoC_shru16_ri(REG6,1,DEFAULT);
    VoC_multf32_rp(ACC1,REG7,REG1,IN_PARALLEL);
    VoC_multf16_rp(REG6,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_add32_rp(ACC0,ACC0,REG4,DEFAULT);
    VoC_mac32_rr(REG6,REG7,IN_PARALLEL);    // here reg7=1
    VoC_movreg16(REG6,REG2,DEFAULT);
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);
    VoC_sub16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_sw32_p(ACC0,REG6,DEFAULT);
    VoC_endloop0
    VoC_lw32_p(ACC0,REG2,DEFAULT);
Get_lsp_pol_L1:
    VoC_lw16_d(REG4,CONST_512_ADDR);//gugogi
    VoC_msu32inc_rp(REG4,REG1,DEFAULT);
    VoC_movreg16(INC2,REG0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sub16_rr(REG4,REG2,REG7,DEFAULT);   // here reg7=4
    VoC_endloop1

    VoC_return;
}


/******************************************
  Function: CII_Lsp_Az
  Input:  &lsp[] -> REG1
     &Az[]  -> REG0

   Optimized by Kenneth  09/13/2004
******************************************/

void CII_Lsp_Az(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push32(REG01,DEFAULT);
    VoC_lw16i_set_inc(REG2,F1_ADDR,2);
    VoC_jal(CII_Get_lsp_pol);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16i_set_inc(REG2,F2_ADDR,2);
    VoC_add16_rd(REG1,REG1,CONST_1_ADDR);
    VoC_jal(CII_Get_lsp_pol);

    VoC_lw16i_set_inc(REG0,F1_ADDR+6*2-2,-2);/*REG0 is a point to f1[i]*/
    VoC_lw16i_set_inc(REG2,F1_ADDR+5*2-2,-2);/*REG2 is a point to f1[i-1]*/
    VoC_lw16i_set_inc(REG1,F2_ADDR+6*2-2,-2);/*REG1 is a point to f2[i]*/
    VoC_lw16i_set_inc(REG3,F2_ADDR+5*2-2,-2);/*REG3 is a point to f2[i-1]*/
    VoC_loop_i_short(5,DEFAULT)
    VoC_lw16i_short(REG7,10,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32_p(ACC0,REG0,DEFAULT);//f1[i] = L_add (f1[i], f1[i - 1])
    VoC_add32inc_rp(ACC0,ACC0,REG2,DEFAULT);
    VoC_lw32_p(ACC1,REG1,DEFAULT); //CHANGE
    VoC_sub32inc_rp(ACC1,ACC1,REG3,DEFAULT);//f2[i] = L_sub (f2[i], f2[i - 1])
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_endloop0
    VoC_lw16i(REG4,4096);

    VoC_pop32(REG23,DEFAULT);

    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);  // reg7=10
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,F1_ADDR+2,2);
    VoC_lw16i_set_inc(REG1,F2_ADDR+2,2);

    VoC_loop_i_short(5,DEFAULT)
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_add32_rr(ACC0,RL7,RL6,DEFAULT);
    // VoC_lw16i_short(REG5,12,IN_PARALLEL);
    // VoC_jal(CII_L_shr_r);
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_sub32_rr(ACC0,RL7,RL6,DEFAULT);
    exit_on_warnings = OFF;
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    exit_on_warnings = ON;
    // VoC_lw16i_short(REG5,12,IN_PARALLEL);
    // VoC_jal(CII_L_shr_r);
    VoC_shr32_ri(ACC0,12,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_1_ADDR);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(ACC0_LO,REG3,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_return;
}

/**
PARAMETERS:
R0: &lsf inc 1
R1: &lsp inc 1

*****/

void CII_Lsf_lsp(void)
{
    /*&lsf and &lsp*/
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,TABLE_LSP_LSF_ADDR,1);       //&table[0]


    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_loop_i_short(M,DEFAULT)
    VoC_startloop0
    VoC_movreg16(REG7,REG3,DEFAULT);

    VoC_shr16_ri(REG3,8,DEFAULT);       //ind

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);   //&table[ind]
    VoC_and16_ri(REG7,0XFF);            //offset

    VoC_lw16inc_p(REG5,REG3,DEFAULT);       //&table[ind+1]
    VoC_sub16_pr(REG6,REG3,REG5,DEFAULT);
    VoC_multf32_rr(REG67,REG6,REG7,DEFAULT);
    VoC_NOP();
    VoC_shr32_ri(REG67,9,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG6,DEFAULT);
    VoC_lw16inc_p(REG3,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
}


/**
PARAMETERS:
R0: &lsp inc -1;
R1: &lsf inc -1;
Optimised by cui 2005.01.19
***/

void CII_Lsp_lsf (void)
{
    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,63,-1);       //for ind
    VoC_lw16i_set_inc(REG3,(TABLE_LSP_LSF_ADDR+63),-1);

    VoC_lw16i(REG4,TABLE_SLOPE_ADDR+63);   //&slope[0]
    VoC_loop_i_short(M,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_loop_i(1,64)
    VoC_sub16inc_rp(REG7,REG6,REG3,DEFAULT);
    VoC_bngtz16_r(LSP_LSF_02,REG7)
    VoC_inc_p(REG2,DEFAULT);
    VoC_sub16_rd(REG4, REG4,CONST_1_ADDR);
    VoC_endloop1
LSP_LSF_02:

    VoC_multf32_rp(REG67,REG7,REG4,DEFAULT);
    VoC_movreg16(REG5,REG2,DEFAULT); //ind
    VoC_shr32_ri(REG67,-3,DEFAULT);
    VoC_shr16_ri(REG5,-8,IN_PARALLEL);  //ind<<8
    VoC_add32_rd(REG67,REG67,CONST_0x00008000_ADDR);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);
    VoC_add16_rd(REG3,REG3,CONST_1_ADDR);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_return
}

// REG0 FOR XN[], REG1 FOR H[] REG3 FOR Y[]
void CII_Convolve(void)
{


    VoC_movreg32(REG67,REG01,DEFAULT);
    VoC_lw16i_set_inc(REG2,0,1);                //REG2 for n/2
    VoC_sub16_rd(REG3,REG3,CONST_1_ADDR);


    /*RL6:constant 0X00010000*/
    VoC_lw16i_short(RL6_HI,1,DEFAULT);
    VoC_lw16i_short(RL6_LO,0,IN_PARALLEL);
    /*RL7:constant:0xffff0001*/
    VoC_lw16i_short(RL7_HI,-1,DEFAULT);
    VoC_lw16i_short(RL7_LO,1,IN_PARALLEL);

    VoC_multf32_pp(ACC0,REG0,REG1,DEFAULT);

//  exit_on_odd_address = OFF;
    VoC_loop_i(1,20);

    VoC_bngtz16_r(Convolve100,REG2)
    VoC_loop_r_short(REG2,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG1);
    VoC_endloop0
Convolve100:
    VoC_add32_rr(REG01,REG67,RL6,DEFAULT);  //&h[n]++  //REG7++;REG6
    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG67,REG67,RL6,IN_PARALLEL);  //&h[n]++  //REG7++;REG6

    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);                //n/2++

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);                //s=0

    VoC_loop_r_short(REG2,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_add32_rr(REG67,REG67,RL6,DEFAULT);  //&h[n]++
    VoC_sw16inc_p(REG5,REG3,IN_PARALLEL);   //save result of previous loop

    VoC_movreg32(REG45,ACC0,DEFAULT);
    VoC_add32_rr(REG01,REG67,RL7,IN_PARALLEL);  //&x[1] and &h[n+1]

    VoC_shr32_ri(REG45,-3,DEFAULT);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_multf32_pp(ACC0,REG6,REG7,DEFAULT);


    VoC_endloop1
//  exit_on_odd_address = ON;
    VoC_sw16inc_p(REG5,REG3,DEFAULT);

    VoC_return;
}
/*
INPUT:   REG23
OUTPUT:   REG4:EXP;REG5:FRAC

USED:   loop_reg[0]
used:   R01,R23,R45;
*/

void CII_Log2(void)                        /////////////////////revesed by cui 2005.01.18
{

    VoC_lw32z(REG45,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    VoC_push32(REG01,DEFAULT);
    VoC_movreg32(REG23, ACC0, DEFAULT);
    VoC_bgtz32_r(LOG1001,ACC0);    /* if (L_x <= (Word32) 0) goto */
    VoC_lw32z(REG45,DEFAULT);
    VoC_jump(LOG1002);
LOG1001:
    VoC_movreg32(ACC1,ACC0,DEFAULT); //acc1 :ener_code        // save val ACC0 in ACC1    ener_code
    VoC_sub16_dr(REG1,CONST_30_ADDR,REG1);//reg1:exponent
    VoC_shr32_ri(REG23,9,DEFAULT);

    VoC_sub16_rd(REG0,REG3,CONST_32_ADDR);      //reg0:i
    VoC_shr32_ri(REG23,1,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_and16_rd(REG2,CONST_0x7FFF_ADDR);    //reg2:a
    VoC_lw16i(REG4,TABLE_LOG2_ADDR);
    VoC_add16_rr(REG0,REG4,REG0,DEFAULT);           //reg0:&table[i]

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16inc_p(REG5,REG0,DEFAULT);               //REG45:L_y

    VoC_sub16_rp(REG3,REG5,REG0,DEFAULT);           //REG3:tmp
    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);        /* L_y = L_msu (L_y, tmp, a);  */
    VoC_NOP();
    VoC_sub32_rr(REG45,REG45,REG23,DEFAULT);
    VoC_movreg16(REG4,REG1,DEFAULT);        //RETURN:REG45
LOG1002:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}


void CII_Lsf_wt(void)
{
    /*&lsf and &wf*/
    //REG0   *lsf     REG1    *wf
    VoC_movreg32(REG23,REG01,DEFAULT);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_inc_p(REG0,DEFAULT);       // &lsf[1]
    VoC_lw16i_short(REG6, 0,IN_PARALLEL);
    VoC_loop_i_short(9,DEFAULT)
    VoC_startloop0
    VoC_sub16inc_pr(REG7, REG0, REG6,DEFAULT);
    VoC_lw16inc_p(REG6, REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG1, DEFAULT);
    VoC_endloop0
    VoC_sub16_dr(REG6,CONST_16384_ADDR,REG6); /*wf[9] = sub (16384, lsf[8])*/
    VoC_lw16i(REG4,1843);
    VoC_sw16_p(REG6,REG1, DEFAULT);

    VoC_lw16i(REG5,3427);
    VoC_lw16i(REG6,28160);
    VoC_lw16i(REG7,6242);
    VoC_loop_i_short(10,DEFAULT);
    VoC_startloop0

    VoC_multf16_rp(REG0,REG6,REG3,DEFAULT);
    VoC_sub16_pr(REG1,REG3,REG4,DEFAULT);   //REG1: temp
    VoC_sub16_rr(REG0,REG5,REG0,DEFAULT);//REG0 for wf[i]
    VoC_bltz16_r(Lsf_wt101,REG1);
    VoC_multf16_rr(REG0,REG1,REG7, DEFAULT);
    VoC_NOP();
    VoC_sub16_rr(REG0,REG4,REG0,DEFAULT);
Lsf_wt101:
    VoC_shr16_ri(REG0,-3,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG0,REG3, DEFAULT);
    VoC_endloop0
    VoC_return;
}




/*
PARAS:
R0:     &a  INC1
REG3:   &x  INC1
REG2:   &y  INC1

*/
void CII_Residu_new (void)
{
    VoC_lw16inc_p(REG6,REG0,DEFAULT);   // REG6 for a[0]
    VoC_movreg16(RL7_LO,REG0,DEFAULT);
    // REG0 for a[j]
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_loop_i(1,40)
    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_sub16_rd(REG1,REG3,CONST_2_ADDR);   //REG1 for x[i-j]
    VoC_loop_i_short(10,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_endloop0
    VoC_movreg16(REG0,RL7_LO,DEFAULT);
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000_ADDR);
    VoC_NOP();
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop1
    VoC_return;
}

// RL7 for  *mem_syn,*mem_w0,

void CII_subframePostProc(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw32_d(REG67,COD_AMR_GAIN_PIT_ADDRESS); //pitch_fac = gain_pit;
    VoC_lw16i(REG1,STRUCT_COD_AMRSTATE_SHARP_ADDR);
    VoC_bez16_r(subframePostProc103,REG0)
    VoC_lw16i(REG1,COD_AMR_SHARP_SAVE_ADDRESS);
    VoC_lw32_d(REG67,COD_AMR_GAIN_PIT_SF0_ADDRESS); //pitch_fac = gain_pit;
subframePostProc103:

    VoC_movreg16(REG3,REG6,DEFAULT);    // *sharp = gain_pit;
    VoC_push32(REG67,DEFAULT);/* REG6 for gain_pit REG7 for gain_code */
    VoC_lw16i_set_inc(REG2,13017,2);
    VoC_bngt16_rr(subframePostProc102,REG3,REG2)    // if (sub(*sharp, SHARPMAX) > 0)
    VoC_movreg16(REG3,REG2,DEFAULT);            // *sharp = SHARPMAX;
subframePostProc102:


    // tempShift = 1;
    VoC_lw16i_short(FORMAT32,-2,DEFAULT);
    VoC_lw16i_short(REG5,-2,DEFAULT);   // kShift = 2;

    VoC_sw16_p(REG3,REG1,DEFAULT);




    // tempShift = 2;
    VoC_lw16i_short(FORMAT32,-3,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);           // pitch_fac = shr (gain_pit, 1);
    VoC_lw16i_short(REG5,-4,IN_PARALLEL);  // kShift = 4;


    VoC_push16(REG0,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_push16(REG5,DEFAULT);   //save kShift
    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_lw16_sd(REG2,3,DEFAULT);        // REG2 for i_subfr
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COD_AMR_CODE_ADDRESS,1);                 /* REG1 for code[] address */

    VoC_lw16_sd(REG5,1,DEFAULT);   // REG5 for sf or not
    VoC_movreg32(ACC1, RL6,IN_PARALLEL);

    VoC_bez16_r(subframePostProc105,REG5)
    VoC_lw16_d(REG2,COD_AMR_I_SUBFR_SF0_ADDRESS);
    VoC_lw16i_set_inc(REG1,COD_AMR_CODE_SF0_ADDRESS,1);                 /* REG1 for code[] address */
subframePostProc105:

    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);

//  VoC_bez16_d(subframePostProc01, GLOBAL_EFR_FLAG_ADDR)
//
//      VoC_lw16i_short(FORMAT32,-4,DEFAULT);
//      VoC_shr16_ri(REG6,1,DEFAULT);
//subframePostProc01:

    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_mac32_rp(REG6,REG0,DEFAULT);                  /* L_temp = L_mult (exc[i + i_subfr], gain_pit); */
    VoC_multf32inc_rp(REG45,REG7,REG1,DEFAULT);
    VoC_NOP();
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);
    VoC_movreg32(ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG5,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
//   Syn_filt(Aq, &exc[i_subfr], &synth[i_subfr], L_SUBFR, mem_syn, 1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_lw16_sd(REG2,4,DEFAULT);        // REG2 for i_subfr
    VoC_lw16_sd(REG5,6,DEFAULT);    // REG5 for Aq
    VoC_lw16_sd(REG0,2,DEFAULT);    // REG0 for sf or not

    VoC_bez16_r(subframePostProc106,REG0)
    VoC_sub16_rd(REG5,REG5,CONST_11_ADDR);
    VoC_lw16_d(REG2,COD_AMR_I_SUBFR_SF0_ADDRESS);
subframePostProc106:

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_lw16i(REG7,COD_AMR_SYNTH_ADDR);
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);            /* 1 address */
    VoC_lw16i_short(REG6,40,IN_PARALLEL);           /* L_SUBFR address */
    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);

    VoC_pop16(REG1,DEFAULT);        // REG1 for i_subfr
    VoC_lw16i_set_inc(REG2,COD_AMR_SYNTH_ADDR,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);



    VoC_add16_rd(REG0,REG1,CONST_30_ADDR);
    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG0,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,1);

    VoC_loop_i_short(10,DEFAULT)          /* for (j = 0; j < M; j++) */
    VoC_lw16inc_p(REG4,REG3,IN_PARALLEL);       //mem_err[j] = sub(speech[i_subfr + i], synth[i_subfr + i]);
    VoC_startloop0
    VoC_sub16inc_rp(REG4,REG4,REG2,DEFAULT);
//      VoC_sub16inc_rp(REG5,REG5,REG2,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG3,DEFAULT);       //mem_err[j] = sub(speech[i_subfr + i], synth[i_subfr + i]);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_pop16(RL6_LO,DEFAULT);      // RL6_LO for kShift
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_XN_ADDRESS+30,1);                    /* REG0 for xn[] address */
    VoC_lw16i_set_inc(REG2,COD_AMR_Y2_ADDRESS+30,1);                    /* REG2 for y2[] address */
    VoC_bez16_r(subframePostProc107,REG1)
    VoC_lw16i_set_inc(REG0,COD_AMR_XN_SF0_ADDRESS+30,1);                    /* REG0 for xn[] address */
    VoC_lw16i_set_inc(REG2,COD_AMR_Y2_SF0_ADDRESS+30,1);                    /* REG2 for y2[] address */
subframePostProc107:
    VoC_movreg16(REG3,RL7_LO,DEFAULT);
    VoC_lw16i_set_inc(REG1,COD_AMR_Y1_ADDRESS+30,1);                    /* REG1 for y1[] address */


    /* REG3 for mem_w0 address */
    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_loop_i_short(10,DEFAULT)          /* for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++) */
    VoC_lw32_sd(REG67,0,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(REG45,REG6,REG1,DEFAULT);
    VoC_multf32inc_rp(REG67,REG7,REG2,DEFAULT);         /* k = extract_h (L_shl (L_mult (y2[i], gain_code), 5)); */

    VoC_shr32_ri(REG45,-1,DEFAULT);         /* REG5 for temp temp = extract_h(L_shl(L_mult(y1[i], gain_pit), 1)); */
    VoC_movreg16(REG4, RL6_LO, DEFAULT);      // REG4 for kShift
//      VoC_bez16_d(subframePostProc02,GLOBAL_EFR_FLAG_ADDR)
//      VoC_sub16_rd(REG4,REG4,CONST_1_ADDR);
//subframePostProc02:
    VoC_shr32_rr(REG67,REG4,DEFAULT);               /* REG7 for k */
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);

    VoC_sub16inc_pr(REG4,REG0,REG7,DEFAULT);    /*  mem_w0[j] = sub (xn[i], add (temp, k));  */
    VoC_lw32_sd(REG67,0,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);       /* save mem_w0 */
    VoC_endloop0

//subframePostProc302:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG67,DEFAULT);
    /*    {
          temp = extract_h(L_shl(L_mult(y1[i], gain_pit), 1));
          k = extract_h(L_shl(L_mult(y2[i], gain_code), kShift));
          mem_w0[j] = sub(xn[i], add(temp, k));
       }
    */
    VoC_return;
}




/*
PARAS:
R0: &a      INC1
R1: &fac    INC1
R2: &a_exp  INC1
*/


void CII_Weight_Ai (void)
{


    VoC_lw16inc_p(REG5,REG0,DEFAULT);           //&a[1]

    VoC_loop_i_short(11,DEFAULT)
    VoC_startloop0


    VoC_multf32inc_pp(REG45,REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_add32_rd(REG45,REG45,CONST_0x00008000_ADDR);

    VoC_endloop0

    VoC_return;

}


/***********************************************
  Function CII_Int_lpc_only
  input: REG2->lsp_mid
         REG3->lsp_old
         REG1(incr=2)->&lsp[] push(n)
  Optimized by Kenneth  18/09/2004
***********************************************/
void CII_Int_lpc_only(void)
{

    VoC_lw16i_short(INC3,2,DEFAULT);
    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);      //lsp_mid
    VoC_lw32inc_p(REG45,REG2,DEFAULT);      //lsp_old
    VoC_loop_i_short((M/2),DEFAULT)
    VoC_startloop0
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_shr16_ri(REG6,1,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);          //lsp_mid
    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_lw32inc_p(REG45,REG2,DEFAULT);          //lsp_old
    VoC_endloop0
    VoC_return;

}




// REG0:  fraction
// REG1:  exponent
// REG23: return
//used:REG0,1,2,3,4,5
void CII_Pow2 (void)
{

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_multf32_rd(REG23,REG0,CONST_32_ADDR);
    VoC_lw16i_set_inc(REG0,TABLE_POW2_ADDR,1);
    VoC_add16_rr(REG0,REG0,REG3,DEFAULT);   //REG0:addr of table[i]
    VoC_shr32_ri(REG23,1,IN_PARALLEL);
    VoC_and16_ri(REG2,0x7fff);          //reg2:a

    VoC_lw16inc_p(REG5,REG0,DEFAULT);       //REG45:L_x



    VoC_sub16_rp(REG3,REG5,REG0,DEFAULT);   //REG3:tmp

    VoC_multf32_rr(REG23,REG3,REG2,DEFAULT);
    VoC_sub16_dr(REG0,CONST_30_ADDR,REG1); //exp = sub (30, exponent);
    VoC_sub32_rr(REG23,REG45,REG23,DEFAULT);


//  VoC_lw16i_short(REG4,31,DEFAULT);//L_shr_r(r)
    VoC_bngt16_rd(POW2101,REG0,CONST_31_ADDR);
    VoC_lw32z(REG23,DEFAULT);
    VoC_return

POW2101:

    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);
    VoC_shr32_rr(REG23,REG0,DEFAULT); //L_shr_r = (L_shr(r-1) +1) >> 1
    VoC_bngt16_rd(POW2102,REG0,CONST_1_ADDR);
    VoC_add32_rd(REG23,REG23,CONST_0x00000001_ADDR);

POW2102:
    VoC_shr32_ri(REG23,1,DEFAULT);//L_shr_r(r) = (L_shr(r-1) +1) >> 1

    VoC_return;
}




/************************************************************************* * *
FUNCTION:  CII_gc_pred_update() * Input:  *st->          REG5
           Word16 *ener_avg_MR122,   REG7                   Word16 *ener_avg          REG6

           Notise:  REG0,REG1,REG2,REG3 increment stage changed to -1
           Created by  Kenneth   07/14/2004
           *************************************************************************/
void CII_gc_pred_update(void)
{
    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_add16_rr(REG0,REG5,REG3,DEFAULT);
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG2,REG5,CONST_6_ADDR);
    VoC_add16_rr(REG1,REG0,REG3,DEFAULT);
    VoC_add16_rr(REG3,REG2,REG3,DEFAULT);
    //addr of past_qua_en[2]
    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);//addr of past_qua_en[3]


    VoC_lw16i_short(INC2,-1,DEFAULT);   //addr of qua_ener_MR122[2]
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);//addr of qua_ener_MR122[3]
    VoC_loop_i_short(3,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG0,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0;
    VoC_sw16_p(REG7,REG3,DEFAULT);
    VoC_sw16_p(REG6,REG1,DEFAULT);
    VoC_return;
}




void CII_Autocorr(void)
{

    /* Windowing of signal */
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG2,AUTOCORR_Y_ADDR-2,1);                    //&y
    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_loop_i(0,(L_WINDOW/2)+1)
    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_multf32inc_pp(ACC0,REG0,REG1,IN_PARALLEL);
    VoC_add32_rr(REG67,ACC1,RL6,DEFAULT);
    VoC_multf32inc_pp(ACC1,REG0,REG1,IN_PARALLEL);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_endloop0


    /* Compute r[0] and test for overflow */
    VoC_lw16i_short(RL6_LO,0,DEFAULT);      //RL6_LO for overfl_shft
    VoC_lw16i_short(RL7_LO,4,IN_PARALLEL);


AUTOCORR_01:

    VoC_lw32z(ACC0,DEFAULT);                            //s=0
    VoC_lw16i_set_inc(REG1,AUTOCORR_Y_ADDR,2);                          //&y

    VoC_loop_i(0,L_WINDOW/2)
    VoC_bimac32inc_pp(REG1, REG1);
    VoC_endloop0

    VoC_NOP();
    /* If overflow divide y[] by 4 */
    VoC_bne32_rd(AUTOCORR_01_END,ACC0,CONST_0x7FFFFFFF_ADDR);   //? sum==MAX_32

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);

    VoC_lw16i_set_inc(REG0,AUTOCORR_Y_ADDR,2);                          //&y
    VoC_movreg16(REG1,REG0,DEFAULT);
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_lw16i_short(REG3,60,IN_PARALLEL);
    VoC_jal(CII_scale);

    VoC_jump(AUTOCORR_01)


AUTOCORR_01_END:


    VoC_add32_rd (ACC0, ACC0,CONST_0x00000001_ADDR);    //s+=lL

    /* Normalization of r[0] */
    VoC_jal(CII_NORM_L_ACC0);


    //REG67 for sum,NORMLIZATION IS DONE IN NORM_L
    VoC_movreg16(REG5,RL6_LO,DEFAULT);          //over_shft
    VoC_movreg32(REG67,ACC0,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG1,REG5,DEFAULT);       //norm-over_shft

    VoC_shru16_ri(REG6,1,IN_PARALLEL);
//  VoC_and16_ri(REG6,0X7FFF);
    VoC_sw16_d(REG7,LPC_RHIGH_ADDR);
    VoC_sw16_d(REG6,LPC_RLOW_ADDR);
    VoC_sw16_d(REG5,SCAL_ACF_ADDR_P);

    VoC_sub16_dr(REG4,CONST_0_ADDR,REG1);       //REG4:norm=-norm

    /*copy y(in RAM_X) to y_copy(in RAM_Y)*/
    VoC_lw16i_set_inc(REG0,AUTOCORR_Y_ADDR,2);                          //&y
    VoC_lw16i_set_inc(REG1,AUTOCORR_Y_COPY_ADDR,2);                 //&y_copy
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(REG6,60,IN_PARALLEL);
    VoC_jal(CII_copy_xy);
    VoC_sw32_p(RL6,REG1,DEFAULT);


    VoC_lw16i_short(INC2,2,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);
    /* r[1] to r[m] */
    VoC_lw16i_set_inc(REG0,LPC_RLOW_ADDR+1,1);      //&r_l[]
    VoC_lw16i_set_inc(REG1,LPC_RHIGH_ADDR+1,1);     //&r_h[]

    // here REG4 for norm
    VoC_lw16i(RL6_HI, AUTOCORR_Y_COPY_ADDR+1);                  //&y_copy[1]
    VoC_lw16i(RL6_LO,240);//L_WINDOW

    VoC_lw32_d(RL7,CONST_0x0000FFFF_ADDR);      //0X0000FFFF

    VoC_pop16(RA,DEFAULT);


    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg16(REG3,RL6_HI,IN_PARALLEL);          //REG3 addr of y_copy[j+i]
    VoC_lw16i(REG2,AUTOCORR_Y_ADDR);                        //REG2 addr of y[j]
    VoC_movreg16(REG5,RL6_LO,DEFAULT);
    VoC_aligninc_pp(REG2,REG3,DEFAULT);
//  exit_on_odd_address = OFF;

    VoC_shr16_ri(REG5,1,IN_PARALLEL);


    VoC_loop_i(1,10);

    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG3);
    VoC_endloop0;

    VoC_add32_rr(RL6,RL6,RL7,DEFAULT);
    VoC_lw16i(REG2,AUTOCORR_Y_ADDR);
    //REG2 addr of y[j]
    VoC_shr32_rr(ACC0,REG4,DEFAULT);
    VoC_movreg16(REG3,RL6_HI,IN_PARALLEL);//REG3 addr of y_copy[j+i]
    VoC_movreg32(REG67,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_aligninc_pp(REG2,REG3,DEFAULT);
    VoC_shru16_ri(REG6,1,IN_PARALLEL);

//      VoC_and16_ri(REG6,0X7FFF);
    VoC_sw16inc_p(REG7, REG1, DEFAULT);
    VoC_movreg16(REG5,RL6_LO,IN_PARALLEL);

    VoC_sw16inc_p(REG6, REG0, DEFAULT);
    VoC_shr16_ri(REG5,1,IN_PARALLEL);

    VoC_endloop1;
//  exit_on_odd_address = ON;

    VoC_return;
}


/*******************************
 Function: CII_Chebps
 Input:  REG2->&f[0]
         REG3->x
 Output: REG7->cheb
         REG0<-0x7fff
         REG1<-0x2000
 Used  : all except RL67
 unchanged : REG3
 Optimized by Yordan 08/12/2005
********************************/


void CII_Chebps (void)
{

    VoC_lw32z(ACC1,DEFAULT);
    VoC_lw16i(REG0,0x7fff);
    VoC_lw16_d(REG1,CONST_8192_ADDR_Y);
    VoC_lw16i(REG6,CHEBPS_B2_ADDR);
    VoC_lw16i(REG7,CONST_0x8000FFFF_ADDR);
//  VoC_lw32_d(ACC1,CONST_0x1000000_ADDR);


    VoC_lw16i(ACC1_HI,0x100);
    // b1 = 2*x + f[1]
    VoC_multf32_rd(REG45,REG3,CONST_512_ADDR);
    VoC_multf32inc_rp(ACC0,REG1,REG2,DEFAULT);
    VoC_sw32_p(ACC1,REG6,DEFAULT);
    VoC_add32_rr(REG45,REG45,ACC0,DEFAULT);


    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG0,DEFAULT);

    VoC_loop_i_short(3,DEFAULT)
    VoC_startloop0
    VoC_multf32_rr(ACC1,REG4,REG3,DEFAULT); //b1_l*x
    // t0 = 2.0*x*b1
    VoC_multf32inc_rp(ACC0,REG1,REG2,DEFAULT);  //+f[i];

    VoC_shr32_ri(ACC1,16,DEFAULT);
    VoC_shr32_ri(ACC1,-1,DEFAULT);
    exit_on_warnings = OFF;
    VoC_mac32_rr(REG5,REG3,IN_PARALLEL);    //b1_h*x+b1_l*1<<1
    exit_on_warnings = ON;

    VoC_bimac32inc_pp(REG6,REG7);           // -b2
    VoC_sw32_p(REG45,REG6,DEFAULT);         //B2=B1;REG45: free
    VoC_shr32_ri(ACC1,-1,IN_PARALLEL);      //b1*x*2
    exit_on_warnings = OFF;
    VoC_add32_rr(REG45,ACC1,ACC0,DEFAULT);      //-b2+f[i]+2*b1*x
    exit_on_warnings = ON;
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_and16_rr(REG4,REG0,DEFAULT);

    VoC_endloop0

    // t0 = x*b1;
    VoC_multf32_rr(ACC0,REG4,REG3,DEFAULT); //b1_l*x
    VoC_NOP();
    VoC_shr32_ri(ACC0,15,DEFAULT);          //b1_h*x+b1_l*x<<1
    VoC_bimac32inc_pp(REG6,REG7);           //-b2

    VoC_mac32_pd(REG2,CONST_4096_ADDR);     //+f[i]/2
    VoC_multf32_rr(REG67,REG5,REG3,DEFAULT);//b1_h*x
    VoC_NOP();
    VoC_add32_rr(REG67,REG67,ACC0,DEFAULT);

    VoC_shr32_ri(REG67,-6,DEFAULT);
    VoC_return;
}


/*******************************************************
 Function: CII_Az_lsp
 Input:  REG6->x         ---az[MP1+x]
         REG1(incr=-1)->lsp[]
         REG0(incr=1)->old_lsp[]

 Optimized by Kenneth 09/06/2004
********************************************************/


void CII_Az_lsp (void)
{

    VoC_push32(REG01,DEFAULT);      // push32(n)  &lsp[]/&old_lsp[]
    VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS+12,1);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);

    VoC_lw16i_short(REG5,9,IN_PARALLEL);
    VoC_add16_rr(REG1,REG0,REG5,DEFAULT);

    VoC_push16(RA,IN_PARALLEL);         // push16(n)

    VoC_lw16i(REG6,1024);
    VoC_lw16i_set_inc(REG2,AZ_LSP_F1_ADDR,1);                   //&f1
    VoC_lw16i_set_inc(REG3,AZ_LSP_F2_ADDR,1);                   //&f2

    VoC_sw16_p(REG6,REG2,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_sw16_p(REG6,REG3,DEFAULT);
    VoC_lw16i(REG4,8192);

    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_multf32inc_rp(ACC0,REG4,REG0,DEFAULT);
    VoC_multf32inc_rp(REG67,REG4,REG1,DEFAULT);
    VoC_NOP();
//exit_on_warnings=OFF;
    VoC_sub32_rr(REG67,ACC0,REG67,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,REG67,IN_PARALLEL);
//exit_on_warnings=ON;
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
    VoC_add16inc_rp(REG7,REG7,REG3,IN_PARALLEL);
    VoC_sub16inc_rp(REG5,REG5,REG2,DEFAULT);
    VoC_sw16_p(REG7,REG3,DEFAULT);
    VoC_sw16_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i(REG2,AZ_LSP_F1_1_ADDR);
    VoC_sw16_d(REG4,AZ_LSP_IP_ADDR);

    VoC_lw16i(REG6,TABLE_GRID_ADDR);
    VoC_lw16i(REG7,(TABLE_GRID_ADDR+61));

    VoC_lw16_p(REG3,REG6,DEFAULT);

    VoC_push16(REG4,DEFAULT);       // push16(n-1)  ylow
    VoC_push32(REG67,IN_PARALLEL);  // push32(n-1)  &grid[61](REG7)/&grid[i](REG6)

    VoC_push16(REG4,DEFAULT);       // push16(n-2)  nf
    VoC_push32(REG23,IN_PARALLEL);  // push32(n-2)  xlow(REG3)/coef(REG2)

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);
    VoC_sw16_sd(REG7,1,DEFAULT);    // restore  ylow

AZ_LSP_LOOP:

    VoC_lw32_sd(REG23,0,DEFAULT);           // xlow(REG3)/coef(REG2)
    VoC_lw16i_short(REG0,10,IN_PARALLEL);

    VoC_lw32_sd(REG67,1,DEFAULT);           // &grid[61](REG7)/&grid[i](REG6)
    VoC_lw16_sd(REG4,0,IN_PARALLEL);        // nf

    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);   //j++

    VoC_bngt16_rr(AZ_LSP_LOOP_END,REG7,REG6)
    VoC_bngt16_rr(AZ_LSP_LOOP_END,REG0,REG4)

    VoC_lw16_sd(REG5,1,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_lw16_p(REG3,REG6,DEFAULT);
    VoC_sw32_sd(REG67,1,DEFAULT);
    VoC_sw32_sd(REG23,0,DEFAULT);           // store xlow
    VoC_push32(REG45,DEFAULT);          // push32(n-3) yhigh(high)/xhigh(low)

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);

    VoC_sw16_sd(REG7,1,DEFAULT);            // restore  ylow
    VoC_lw32_sd(REG45,0,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);     //  L_mult (ylow, yhigh)
    VoC_NOP();
    VoC_bgtz32_r(AZ_LSP_NOLOOP1,ACC0)

    VoC_loop_i(1,4)

    // xmid = (xlow + xhigh)/2
    VoC_lw32_sd(REG23,1,DEFAULT);   // load xlow/coef
    VoC_shr16_ri(REG3,1,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);

    VoC_lw16_sd(REG4,1,DEFAULT);    // load ylow
    VoC_multf32_rr(ACC0,REG7,REG4,DEFAULT);
    VoC_movreg16(REG6,REG3,DEFAULT);

    VoC_bgtz32_r(AZ_LSP_03_01_ELSE,ACC0)
    VoC_sw32_sd(REG67,0,DEFAULT);   // restore  yhigh/xhigh
    VoC_jump(AZ_LSP_03_01_END)
AZ_LSP_03_01_ELSE:
    VoC_lw32_sd(REG23,1,DEFAULT);
    VoC_movreg16(REG3,REG6,DEFAULT);
    VoC_sw16_sd(REG7,1,DEFAULT);    // restore ylow
    VoC_sw32_sd(REG23,1,DEFAULT);   // restore xlow/coef
AZ_LSP_03_01_END:
    VoC_lw32_sd(REG45,0,DEFAULT);   // load yhigh/xhigh

    VoC_endloop1

    // xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);
    VoC_lw32_sd(REG23,1,DEFAULT);           // load  xlow/coef
    VoC_lw16_sd(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG4,REG3,DEFAULT);       //x
    VoC_sub16_rr(REG7,REG5,REG7,IN_PARALLEL);   //y;width 2

    VoC_bez16_r(AZ_LSP_03_02_END,REG7)             //exit with REG3:xlow;

    VoC_push32(REG67,DEFAULT);          // push32(n-4)     sign(reg7)/x(reg6);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_bnltz16_r(AZ_LSP_ABS_S_1,REG7)
    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);
AZ_LSP_ABS_S_1:

    VoC_movreg32(ACC0,REG67,DEFAULT);

    /**************************/
    // input in ACC0
    // output in REG1
    // used ACC0, REG1
    /**************************/

    VoC_jal(CII_NORM_L_ACC0);

    VoC_sub16_dr(REG1,CONST_20_ADDR,REG1);
    VoC_lw16i(REG0,0x3fff);

    VoC_push16(REG1,DEFAULT);           // push16(n-3)  20-exp
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL);

    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);

    VoC_pop32(REG67,DEFAULT);           //   pop32(n-4)     sign(reg7)/x(reg6);

    VoC_multf32_rr(ACC0,REG6,REG2,DEFAULT);     //   t0 = L_mult (x, y);
    VoC_pop16(REG5,DEFAULT);            // pop16(n-3)  20-exp
    VoC_shr32_rr(ACC0,REG5,DEFAULT);                //  t0 = L_shr (t0, sub (20, exp));
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);     //y

    VoC_bnltz16_r(AZ_LSP_04_01_ELSE,REG7);
    VoC_sub16_dr(REG6,CONST_0_ADDR,REG6);
AZ_LSP_04_01_ELSE:

    VoC_lw16_sd(REG7,1,DEFAULT);            // load ylow
    VoC_multf32_rr(REG67,REG6,REG7,DEFAULT);
    VoC_lw32_sd(REG23,1,DEFAULT);           // load  xlow/coef
    VoC_shr32_ri(REG67,11,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);       //exit with REG3:xlow;

AZ_LSP_03_02_END:

    VoC_lw32_sd(REG67,3,DEFAULT);           // load &lsp[]/&lsp_old[]
    VoC_lw16_sd(REG0,0,DEFAULT);            //  load nf

    VoC_add16_rr(REG7,REG0,REG7,DEFAULT);
    VoC_inc_p(REG0,DEFAULT);            // nf++
    VoC_sw16_p(REG3,REG7,DEFAULT);
    VoC_sw16_sd(REG0,0,DEFAULT);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i(REG2,AZ_LSP_F2_1_ADDR);

    VoC_bez16_d(AZ_LSP_03_03_ELSE,AZ_LSP_IP_ADDR)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i(REG2,AZ_LSP_F1_1_ADDR);
AZ_LSP_03_03_ELSE:

    VoC_sw16_d(REG6,AZ_LSP_IP_ADDR);
    VoC_sw32_sd(REG23,1,DEFAULT);

    /*****************************
     Function: CII_Chebps
     Input:  REG2->&f[0]
             REG3->x
     Output: REG7->cheb
             REG0<-0x7fff
             REG1<-0x2000
     Used  : all except RL67
     unchanged : REG3
    ******************************/

    VoC_jal(CII_Chebps);
    VoC_sw16_sd(REG7,1,DEFAULT);        // restore ylow

AZ_LSP_NOLOOP1:

    VoC_pop32(REG45,DEFAULT);       // pop32(n-3) yhigh(high)/xhigh(low)

    VoC_jump(AZ_LSP_LOOP)

AZ_LSP_LOOP_END:

    VoC_pop16(REG4,DEFAULT);        // pop16(n-2)  nf
    VoC_pop32(REG23,DEFAULT);       // pop32(n-2)  xlow(REG3)/coef(REG2)
    VoC_pop16(REG5,DEFAULT);        // pop16(n-1)  ylow
    VoC_pop32(REG67,IN_PARALLEL);       // pop32(n-1)  &grid[61](REG7)/&grid[i](REG6)

    VoC_lw32_sd(REG01,0,DEFAULT);

    VoC_bnlt16_rd(AZ_LSP_01_02,REG4,CONST_10_ADDR)

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_jal(CII_Copy_M);
AZ_LSP_01_02:

    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}




void CII_cbseach_subr1(void)
{
    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_lw16_d(REG4,COD_AMR_T0_ADDRESS);
    VoC_sub16_dr(REG5,CONST_40_ADDR,REG4);

    VoC_add16_rr(REG1,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    VoC_bngtz16_r(cbsearch_10000,REG5)
    VoC_multf16inc_rp(REG4,REG3,REG0,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT);
    VoC_startloop0
    VoC_add16_rp(REG4,REG4,REG1,DEFAULT);
    VoC_multf16inc_rp(REG4,REG3,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
cbsearch_10000:

    VoC_return;
}



/*************************************************************************
 *
 *  FUNCTION:  cor_h_x()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *    The correlation is given by:
 *       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
 *
 *    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
 *    to each position track does not saturate.
 *
 *************************************************************************/


//ACC1_lo  : sf
//REG4  : nb_track & step
//REG5  : 40/step
//incr0 and  incr1 =1

void CII_cor_h_x(void)
{
    /*********  SPEED OPTIMIZATION *********************
    // total gain through using bimac is about 0.1 Mcps
    ****************************************************/


    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG0,40,IN_PARALLEL);

    // stack16: RA, xn2, st->h1, st->sharp, gain_pit, res2, code, y2, subfrNr, anap,  RA, RA

    VoC_lw16_sd(ACC1_HI,9,DEFAULT) ;    //    st->h1
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16_sd(REG6,10,DEFAULT);       // xn2
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw32z(REG23,IN_PARALLEL); // REG3 : k


    /*********  SPEED OPTIMIZATION *********************
    // ptr to first line after buffer
    VoC_add16_rd(REG7,REG6,CONST_40_ADDR);
    VoC_set_incr_i(REG0,2,DEFAULT);
    VoC_set_incr_i(REG1,2,IN_PARALLEL);
    // replace line with 0, and keep erased val on stack
    VoC_lw32_p(RL6,REG7,DEFAULT);
    VoC_sw32_p(RL7,REG7,DEFAULT); //0
    VoC_push32(RL6,DEFAULT);
    ****************************************************/


    VoC_lw16i_short(RL7_LO,5,DEFAULT); //   RL7     tot
    VoC_movreg16(INC2,REG4,IN_PARALLEL);
cor_h_x_loop_begin:

    VoC_lw32z(RL6,DEFAULT);         // max = 0;
    VoC_sub16_rr(REG7,REG0,REG2,IN_PARALLEL);

    VoC_loop_r(1,REG5)

    VoC_add16_rr(REG0,REG2,REG6,DEFAULT);   // REG0 x[j]
    VoC_movreg16(REG1,ACC1_HI,IN_PARALLEL); // REG1 h[0]

    /*********  SPEED OPTIMIZATION *********************
    // execute one more bimac in case of odd loop nb
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_aligninc_p(REG0,DEFAULT);
    VoC_shr16_ri(REG7,1,IN_PARALLEL);
    ****************************************************/

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_add16_rr(REG7,REG2,REG2,IN_PARALLEL);    //  REG3 -> Y32     and fist address of  Y32  must be 0
    VoC_startloop0
//              VoC_bimac32inc_pp(REG0,REG1);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG0,COR_H_X_Y32_ADDR);
    VoC_inc_p(REG2,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG0,IN_PARALLEL);

    VoC_lw16i(REG0,40);
    VoC_sw32_p(ACC0,REG7,DEFAULT);


    VoC_bnltz32_r(cor_h_x_250,ACC0)
    VoC_sub32_dr(ACC0,CONST_WORD32_0_ADDR,ACC0);
cor_h_x_250:

    VoC_sub16_rr(REG7,REG0,REG2,DEFAULT);

    VoC_bngt32_rr(cor_h_x_200,ACC0,RL6)
    VoC_movreg32(RL6,ACC0,DEFAULT);
cor_h_x_200:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_endloop1

    VoC_shr32_ri(RL6,1,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_add32_rr(RL7,RL6,RL7,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL); //REG2 : i

    VoC_bgt16_rr(cor_h_x_loop_begin,REG4,REG3)

    /*********  SPEED OPTIMIZATION *********************
    // get erased value
    VoC_pop32(RL6,DEFAULT);
    VoC_add16_rr(REG7,REG6,REG0,IN_PARALLEL);
    VoC_set_incr_i(REG0,1,DEFAULT);
    VoC_set_incr_i(REG1,1,IN_PARALLEL);
    // restitute erased value
    VoC_sw32_p(RL6,REG7,DEFAULT);
    ****************************************************/

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);
    VoC_movreg32(ACC0,RL7,IN_PARALLEL);

    VoC_jal(CII_NORM_L_ACC0);

    VoC_pop16(RA,DEFAULT);
    VoC_sub16_rr(REG2,REG7,REG1,DEFAULT);

    //// _sd mode
    VoC_lw16i_set_inc(REG0,COR_H_X_Y32_ADDR,2);             //&y32
    VoC_lw16i_set_inc(REG1,C1035PF_DN_ADDR,1);              //&dn
    VoC_lw32_d(ACC1,CONST_0x00008000_ADDR);     //&0x8000

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_loop_i_short(40,DEFAULT)                /*    for (i = 0; i < L_CODE; i++)  */
    VoC_shr32_rr(REG45,REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_add32_rr(ACC0,REG45,ACC1,DEFAULT);
    VoC_lw32inc_p(REG45,REG0,IN_PARALLEL);
    VoC_shr32_rr(REG45,REG2,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG1,DEFAULT);
    VoC_endloop0

    VoC_return;
}


/*************************************************************************
 *
 *  FUNCTION:  cor_h()
 *
 *  PURPOSE:  Computes correlations of h[] needed for the codebook search;
 *            and includes the sign information into the correlations.
 *
 *  DESCRIPTION: The correlations are given by
 *         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
 *
 *  and the sign information is included by
 *         rr[i][j] = rr[i][j]*sign[i]*sign[j]
 *
 *************************************************************************/


void CII_cor_h( void)
{

    VoC_push16(RA,DEFAULT);
    // stack16: RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA,RA

    VoC_lw16i_short(ACC0_LO,2,DEFAULT);

    VoC_lw16_sd(REG2,9,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);
    VoC_loop_i_short(20,DEFAULT);                       /*     for (i = 0; i < L_CODE; i++)*/
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG2,REG2);               /*s = L_mac (s, h[i], h[i]); */
    VoC_endloop0

    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_bne16_rd(COR_H101,REG1,CONST_0x7FFF_ADDR)                   /* if (j == 0) */

    VoC_lw16_sd(REG0, 9, DEFAULT);                                                    /* REG0 for h[] address */
    VoC_lw16i_set_inc(REG1,COR_H_H2_ADDR,2);                            /* REG1 for h2[] address */
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);
    VoC_jal(CII_scale);
    VoC_jump(COR_H102);

COR_H101:

    VoC_shr32_ri(REG01,1,DEFAULT);              /* s = L_shr (s, 1); */
    VoC_jal(CII_Inv_sqrt);

    VoC_shr32_ri(REG01,-7,DEFAULT);             /* k = extract_h (L_shl (Inv_sqrt (s), 7)); */
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i(REG0,32440);
    VoC_multf16_rr(REG5,REG1,REG0,DEFAULT);     /*REG5 for k k = mult (k, 32440); k = 0.99*k */
    /* REG0 for h[] address */
    VoC_lw16_sd(REG0,9,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COR_H_H2_ADDR,2);

    VoC_lw16i_short(FORMAT32,-10,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_multf32inc_rp(ACC1,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_add32_rr(ACC0,ACC0,RL6,DEFAULT);
    VoC_loop_i_short(20,DEFAULT)                        /* for (i = 0; i < L_CODE; i++) */
    VoC_add32_rr(ACC1,ACC1,RL6,IN_PARALLEL);
    VoC_startloop0

    VoC_multf32inc_rp(ACC0,REG5,REG0,DEFAULT);  /* L_mult (h[i], k) */
    VoC_movreg16(ACC1_LO,ACC0_HI,IN_PARALLEL);
    VoC_NOP();
    VoC_add32_rr(ACC0, ACC0,RL6,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG5,REG0,IN_PARALLEL);  /* L_mult (h[i], k) */
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_add32_rr(ACC1, ACC1,RL6,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0

    VoC_lw16i_short(FORMAT32,-1,DEFAULT);
COR_H102:

    /*compute correlations
    VoC_lw16i_set_inc(REG0,C1035PF_SIGN_ADDR,2);
    VoC_lw16i_set_inc(REG1,COPY_C1035PF_SIGN_ADDR,2);
    VoC_jal(CII_copy_xy);
    */
    /*copy h2 to RAM_Y*/
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_lw16i_set_inc(REG0,COR_H_H2_ADDR,2);
    VoC_lw16i_set_inc(REG1,COR_H_H2_COPY_ADDR,2);

    VoC_jal(CII_copy_xy);

    VoC_lw16i_set_inc(REG0,COR_H_H2_ADDR,1);
    VoC_lw16i_set_inc(REG1,COR_H_H2_COPY_ADDR,1);
    VoC_lw16i_set_inc(REG2,(C1035PF_RR_DIAG_ADDR+39) ,-1 );

    VoC_multf32inc_pp(ACC0,REG0,REG1,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_add32_rr(REG67,ACC0,RL6,DEFAULT);

    VoC_mac32inc_pp(REG0,REG1,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0

    VoC_add32_rr(REG45,ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,IN_PARALLEL);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_add32_rr(REG67,ACC0,RL6,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    exit_on_warnings=ON;

    VoC_endloop0

    //_sd mode
    VoC_lw16i(ACC1_LO,COR_H_H2_ADDR);//&h2[0] and &h2_copy[0]
    VoC_lw16i(ACC1_HI,COR_H_H2_COPY_ADDR);

    VoC_lw16i_set_inc(REG3,(C1035PF_RR_SIDE_ADDR+779-38) ,-1  );

    VoC_lw16i_short(RL7_HI,1,DEFAULT);                          //dec=1
    VoC_lw16i_short(RL7_LO,38,IN_PARALLEL);                         //i=38

    VoC_push32(ACC1,DEFAULT);
    VoC_push16(REG3,IN_PARALLEL);

    VoC_lw16i(ACC1_LO,C1035PF_SIGN_ADDR+39);       //sign[j]
    VoC_lw16i(ACC1_HI,C1035PF_SIGN_ADDR);

    VoC_loop_i(1,39)

    VoC_movreg32(REG45,RL7,DEFAULT);
    VoC_lw32_sd(REG01,0,IN_PARALLEL);        //REG0:&h2[0]

    VoC_add16_rr(REG1,REG1,REG5,DEFAULT);       //REG1:&h2_copy[dec]
    VoC_movreg32(REG23,ACC1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_lw16_sd(REG6,0,IN_PARALLEL);

    VoC_sub16_dr(REG7,CONST_40_ADDR,REG5);  //REG7:40-dec:loop number

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);   //REG2:&rr[39][i]
    VoC_lw16i_short(REG5,39,IN_PARALLEL);

    VoC_mac32inc_pp(REG0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_lw16i_short(RL6_HI,1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_push32(RL6,IN_PARALLEL);
    exit_on_warnings=ON;

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    VoC_multf16inc_rp(REG6,REG7,REG3,DEFAULT);
    VoC_movreg16(RL6_LO,REG5,IN_PARALLEL);

    VoC_movreg16(REG7,ACC0_HI,DEFAULT);
    VoC_multf16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_mac32inc_pp(REG0,REG1,DEFAULT);

    VoC_sub32_rr(REG45,REG45,RL6,DEFAULT);

    exit_on_warnings=OFF;
    VoC_sw16_p(REG7,REG4,DEFAULT);
    exit_on_warnings=ON;


    VoC_endloop0

    VoC_pop32(RL6,DEFAULT);
    VoC_add32_rd(RL7,RL7,CONST_0x0000ffffL_ADDR);//dec++;i--

    VoC_endloop1

    VoC_pop32(ACC1,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    VoC_return;
}



void CII_G_pitch(void)
{
    /* divide by 2 "y1[]" to avoid overflow */

    /*start of program*/
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG0,G_PITCH_Y1_ADDR,2);                  //REG0 addr of y1
    VoC_lw16i_set_inc(REG1,G_PITCH_SCALED_Y1_ADDR,2);               //REG1 addr of scaled_y1
    VoC_lw16i_set_inc(REG2,COD_AMR_Y1_ADDRESS,2);

    VoC_lw32_d(ACC1,CONST_1_ADDR);

    VoC_movreg16(RL6_HI,REG0,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);

    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    VoC_movreg16(RL7_LO,REG1,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32inc_p(REG45, REG2, IN_PARALLEL);
    VoC_startloop0;
    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG5,2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45, REG0, DEFAULT);
    VoC_sw32inc_p(REG45, REG1, DEFAULT);
    VoC_lw32inc_p(REG45, REG2, IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;

    VoC_movreg32(REG01,RL6,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_loop_i(1,2)

//      VoC_set_status(STATUS_OVF0,0);              //overflow[0]=0
    VoC_lw16i(STATUS,OVF0_CLR);

    VoC_loop_i_short(20,DEFAULT);
    VoC_movreg32(ACC0,ACC1,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_NOP();

    VoC_bnoe0z16_r(G_PITCH_01_ELSE) //if(overflow == 0
    VoC_jal(CII_NORM_L_ACC0);
    VoC_jump(G_PITCH_01_END);
G_PITCH_01_ELSE:

    VoC_movreg32(REG01,RL7,DEFAULT);
    VoC_movreg32(ACC0,ACC1,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);

    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);   //REG1 for exp_yy-=4

G_PITCH_01_END:

    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);

    VoC_movreg32(REG45,REG23,DEFAULT);
    VoC_movreg32(REG23,ACC0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2);

    VoC_movreg16(REG0,RL6_HI,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);

    VoC_endloop1

    // REG45    exp_yy,  yy
    // REG23    exp_xy,  xy

    VoC_lw16i_short(REG1,15,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_GCOEFF_ADDRESS, 1);

    VoC_sub16_rr(REG6,REG1,REG4,DEFAULT);

    VoC_sw16inc_p(REG5, REG0,DEFAULT);
    VoC_sub16_rr(REG7,REG1,REG2,IN_PARALLEL);

    VoC_sw16inc_p(REG6, REG0,DEFAULT);
    VoC_sub16_rr(REG6,REG2,REG4,IN_PARALLEL);                       //    i = sub (exp_xy, exp_yy);

    VoC_sw16inc_p(REG3, REG0,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);                                //REG2 for 0(gain)

    VoC_sub16_rd(REG1,REG3,CONST_4_ADDR);   //i=xy-4

    VoC_sw16inc_p(REG7, REG0,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);                   // xy=shr(xy,1)

    VoC_bltz16_r(G_PITCH_03_END1,REG1);         //i<0 ?

    VoC_movreg16(REG0,REG3,DEFAULT);
    VoC_movreg16(REG1,REG5,IN_PARALLEL);

    /****************************/
    // input in REG0, REG1
    // output in  REG2
    // used register RL67, REG012
    /****************************/

    VoC_jal(CII_DIV_S);
    //REG2 for gain

    VoC_shr16_rr(REG2,REG6,DEFAULT);    //    gain = shr (gain, i);


    VoC_lw16i(REG1,19661);                              //REG0 for 19661
    VoC_bngt16_rr(G_PITCH_03_END,REG2,REG1);            //?gain>19661
    VoC_movreg16(REG2,REG1,DEFAULT);
G_PITCH_03_END:


    /* clear 2 LSBits */
    VoC_and16_ri(REG2,0XFFFC);              // gain = gain & 0xfffC;
G_PITCH_03_END1:

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG2,COD_AMR_GAIN_PIT_ADDRESS);          //return gain
    VoC_return;

}




void CII_Pre_Process()
{
    VoC_lw16i_set_inc (REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);//REG3指向signal的初始地址
    VoC_lw32_d(REG01,STRUCT_PRE_PROCESSSTATE_X0_ADDR);/* REG0 for x0 REG1 for x1 */
    VoC_lw16i_set_inc (REG2,STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR,1);//REG2指向y1_lo地址
    VoC_lw32_d(REG67,STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR);/* REG6 for y2_lo REG7 for y2_hi */
    VoC_lw32_p(REG45,REG2,DEFAULT);/* REG45 for y1_lo and y1_hi */
    VoC_loop_i(0,160)// loop begin
    /* Mpy_32_16 (y2_hi, y2_lo, a[2]) */

//  VoC_multf32_rd(ACC0,REG7,CONST_0xf16b_ADDR);
//  VoC_multf16_rd(REG6,REG6,CONST_0xf16b_ADDR);

    VoC_lw16i(REG2,0xf16b);
    VoC_multf32_rr(ACC0,REG7,REG2,DEFAULT);
    VoC_multf16_rr(REG6,REG6,REG2,DEFAULT);

    VoC_movreg16(REG2,REG1,DEFAULT);    /* x2 = x1 */
    VoC_mac32_rd(REG6,CONST_1_ADDR);

    VoC_lw16i(REG1,0x1e7f);
    VoC_mac32_rr(REG5,REG1,DEFAULT);
    VoC_multf16_rr(REG6,REG4,REG1,DEFAULT);

//  VoC_mac32_rd(REG5,CONST_0x1e7f_ADDR);
//  VoC_multf16_rd(REG6,REG4,CONST_0x1e7f_ADDR);


    VoC_movreg16(REG1,REG0,DEFAULT);/* x1 = x0 */
    VoC_lw16_p(REG0,REG3,IN_PARALLEL);  /* x0 = signal[i] */
    VoC_mac32_rd(REG6,CONST_1_ADDR);
    VoC_lw16i(REG6,0x076b);
    VoC_lw16i(REG7,0xf12a);

    VoC_mac32_rr(REG0,REG6,DEFAULT);
    VoC_mac32_rr(REG1,REG7,DEFAULT);
    VoC_mac32_rr(REG2,REG6,DEFAULT);


//  VoC_mac32_rd(REG0,CONST_0x076b_ADDR);
//  VoC_mac32_rd(REG1,CONST_0xf12a_ADDR);
//  VoC_mac32_rd(REG2,CONST_0x076b_ADDR);


    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(REG45,ACC0,CONST_0x00008000_ADDR);
    VoC_movreg16(REG4,ACC0_LO,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_shru16_ri(REG4,1,IN_PARALLEL);

    VoC_movreg16(REG5,ACC0_HI,DEFAULT);
//  VoC_and16_ri(REG4,0X7FFF);

    VoC_endloop0
    VoC_sw32_d(REG01,STRUCT_PRE_PROCESSSTATE_X0_ADDR);/* REG0 for x0 REG1 for x1 */
    VoC_sw32_d(REG67,STRUCT_PRE_PROCESSSTATE_Y2_LO_ADDR);/* REG6 for y2_lo REG7 for y2_hi */
    VoC_sw32_d(REG45,STRUCT_PRE_PROCESSSTATE_Y1_LO_ADDR);   /* save y1_lo and y1_hi */

    VoC_return;


}


void CII_subframePreProc(void)
{
    VoC_push16(RA,DEFAULT);
//  VoC_lw16_d(REG6,GLOBAL_ENC_MODE_ADDR);

    VoC_push16(RL7_HI,DEFAULT);
    // A address
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma1_12K2_ADDR,1);      //  g1 = gamma1_12k2;

    /*---------------------------------------------------------------*
     * Find the weighted LPC coefficients for the weighting filter.  *
     *---------------------------------------------------------------*/
    /*
    PARAS:
    R0: &a      INC1
    R1: &fac    INC1
    R2: &a_exp  INC1
    */
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP1_ADDR,1);   //   Weight_Ai(A, g1, Ap1);
    VoC_loop_i(1,2)
    VoC_lw16_sd(REG0,5,DEFAULT);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma2_ADDR,1);  //   Weight_Ai(A, gamma2, Ap2);
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP2_ADDR,1);
    VoC_endloop1
    /*   for (i = 0; i <= M; i++)
    {
       ai_zero[i] = Ap1[i];        move16 ();
    }*/
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_AI_ZERO_ADDR,1);

    VoC_loop_i_short(11,DEFAULT)
    VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0


//  Syn_filt(Aq, ai_zero, h1, L_SUBFR, zero, 0);

    VoC_lw16_sd(REG5,4,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_AI_ZERO_ADDR,1);
    VoC_lw16i(REG7,STRUCT_COD_AMRSTATE_H1_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_ZERO_ADDR,1);
    VoC_jal(CII_Syn_filt);
//   Syn_filt(Ap2, h1, h1, L_SUBFR, zero, 0);
    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_H1_ADDR,1);
    VoC_movreg16(REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_ZERO_ADDR,1);
    VoC_jal(CII_Syn_filt);

    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/


//   Residu(Aq, speech, res2, L_SUBFR);
    /*
    PARAS:
    R0:     &a  INC1
    REG3:   &x  INC1
    REG2:   &y  INC1

    */
    VoC_lw16_sd(REG0,4,DEFAULT);
    VoC_lw16i_short(INC0,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG2,COD_AMR_RES_ADDRESS,1);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);
//  VoC_bez16_d(subframePreProc500, GLOBAL_EFR_FLAG_ADDR)
//  VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,1);
// subframePreProc500:
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_jal(CII_Residu_new);
//   Copy(res2, exc, L_SUBFR);
    /*
    INPUT:
    REG0: source address INC 2
    REG1: dest   address INC 2
    REG6: (number of Long Words copyed)/2 or (number of Word16 copyed)/4

    USED:   ACC0,ACC1
    */
    VoC_lw16i_set_inc(REG0,COD_AMR_RES_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_EXC_ADDR,2);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG6,10,DEFAULT);
    VoC_jal(CII_copy_xy);

//   Syn_filt(Aq, exc, error, L_SUBFR, mem_err, 0);
    VoC_lw16_sd(REG5,4,DEFAULT);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_EXC_ADDR,1);
    VoC_lw16_sd(REG6,2,DEFAULT);    // i_subfr
    VoC_add16_rr(REG3,REG6,REG3,DEFAULT);
    VoC_lw16i(REG7,STRUCT_COD_AMRSTATE_ERROR_ADDR);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_ERROR_ADDR,1);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);

//   Residu(Ap1, error, xn, L_SUBFR);
    /*
    PARAS:
    R0:     &a  INC1
    REG3:   &x  INC1
    REG2:   &y  INC1

    */
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw16i_set_inc(REG2,COD_AMR_XN_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_ERROR_ADDR,1);
    VoC_jal(CII_Residu_new);

//   Syn_filt(Ap2, xn, xn, L_SUBFR, mem_w0, 0);
    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_set_inc(REG3,COD_AMR_XN_ADDRESS,1);
    VoC_movreg16(REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_lw16i_short(INC1,1,DEFAULT);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return
    // end of subframePreProc
}



/**********************************************
  Function CII_build_code_com2
  input: REG6->NB_PULSE
     REG7->h[]
     REG1(incr=1)->codvec[]
     REG0(incr=1)->y[]
     REG4->_sign[]
  Note: This function can used in all mode
  Create by Kenneth  09/29/2004
**********************************************/

void CII_build_code_com2(void)
{
    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,1);
    VoC_lw16i_set_inc(REG3,BUILD_CODE_POINT_ADDR_ADDR,1);
    VoC_lw16i_set_inc(REG2,build_code_sign_ADDR,1);
    VoC_loop_r_short(REG6,DEFAULT);
    VoC_movreg32(RL6,REG23,IN_PARALLEL);
    VoC_startloop0
    VoC_sub16inc_rp(REG5,REG7,REG1,DEFAULT);
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0

    VoC_movreg32(RL7,ACC0,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);

    VoC_loop_i(1,40);

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_startloop0
    VoC_lw16_p(REG1,REG3,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_mac32inc_rp(REG5,REG1,DEFAULT);
    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_endloop0
    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_movreg32(REG23,RL6,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(ACC0_HI,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return;
}




/******************************************************************************
*      File             : amr_dec_main.c

******************************************************************************/

/*---------------------------------------------------------------------------
    Function:  void CII_Pred_lt_3or6(void)
    Word16 exc[],        in/out: REG5
    Word16 T0,           input : REG6
    Word16 frac,         input : REG7
    Word16 L_subfr,      input : subframe size
    Word16 flag3         input : REG2
    Version 1.0
    Version 1.1   revised by Kenneth 08/04/2004
-----------------------------------------------------------------------------*/
void CII_Pred_lt_3or6(void)
{

    VoC_sub16_rr(REG6, REG5,REG6,DEFAULT);//x0 = &exc[-T0];

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_lw16i_short(INC2,6,DEFAULT);
    VoC_lw16i_short(INC3,6,IN_PARALLEL);
    VoC_sub16_dr(REG1,CONST_0_ADDR,REG7);// frac = negate (frac);

    VoC_bez16_r(PRED_IT_3OR6_001,REG2)
    VoC_shr16_ri(REG1,-1,DEFAULT);
PRED_IT_3OR6_001:
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG3,6,IN_PARALLEL);

    VoC_bnltz16_r(PRED_IT6_003,REG1)//if (frac < 0)   REG4   frac
    VoC_add16_rr(REG1,REG1,REG3,DEFAULT);//frac = add (frac, UP_SAMP);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//x0--;
PRED_IT6_003:

    VoC_lw16i(REG7,STATIC_CONST_INTER_6_ADDR);//REG7   intere_6 addr
    VoC_add16_rr(REG2,REG7,REG1,DEFAULT);//c1 = &inter_6[frac]
    VoC_sub16_rr(REG3,REG3,REG1,IN_PARALLEL);   // sub (UP_SAMP_MAX, frac)
    VoC_add16_rr(REG3,REG3,REG7,DEFAULT);//&inter_6[sub (UP_SAMP, frac)];
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    VoC_movreg32(RL7, REG23,DEFAULT);
    VoC_movreg32(RL6,ACC0,IN_PARALLEL);

    VoC_loop_i(1,40);

    VoC_movreg16(REG0,REG6,DEFAULT);    //  x1 = x0++;
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_loop_i_short(10,DEFAULT);
    VoC_movreg16(REG1,REG6,IN_PARALLEL); //   x2 = x0;
    VoC_startloop0
    VoC_mac32inc_pp(REG0,REG2,DEFAULT);
    VoC_mac32inc_pp(REG1,REG3,DEFAULT);
    VoC_endloop0
    VoC_movreg32(REG23,RL7, DEFAULT);
    exit_on_warnings=OFF;
    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);//exc++
    VoC_sw16_p(ACC0_HI,REG5,DEFAULT);//exc[j] = round (s);
    VoC_movreg32(ACC0,RL6,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return;
}


/*_________________________________________________________________
  Function:  CII_Syn_filt
 prameters:
   &a[0]  ->reg5
   &x[0]  -> reg3(inc 1 )
   &y     ->REG7
   lg     ->REG6
   &mem[0]->REG1 (incr=1) push32:
   update ->reg0

 Version 1.0  Create
 Version 1.1 optimized by Kenneth 09/01/2004
____________________________________________________________________*/
void CII_Syn_filt (void)
{
    // Copy mem[] to yy[]
    VoC_push32(REG01,DEFAULT);

    VoC_lw16i_set_inc(REG2,SYN_FILT_TMP_ADDR,1);
    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4,REG2,DEFAULT);           //reg2:yy
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0

//do the filtering
    VoC_lw16_p(REG4,REG5,DEFAULT);          //reg4:a[0]
    VoC_add16_rr(REG5,REG5,REG0,IN_PARALLEL);       //reg5:&a[1]
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

//prepare for the loop

    VoC_lw16i_set_inc(REG1,SYN_FILT_TMP_ADDR+9,-2);//&yy[-1]

    VoC_loop_r(1,REG6)
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
//      exit_on_odd_address = OFF;
    VoC_loop_i_short(5,DEFAULT)
    VoC_multf32inc_rp(ACC0,REG4,REG3,IN_PARALLEL);
    VoC_startloop0
    VoC_bimsu32inc_pp(REG0,REG1);
    VoC_endloop0
    VoC_movreg16(REG0,REG5,DEFAULT);        //&a[1]

    VoC_shr32_ri(ACC0,-3,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_movreg16(REG1,REG2,DEFAULT);    //&yy[-1]
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
//  exit_on_odd_address = ON;
    VoC_endloop1

    VoC_lw16i_set_inc(REG0,SYN_FILT_TMP_ADDR+10,2); // REG0 for tmp[m] address

    VoC_movreg16(REG5,REG6,DEFAULT);        //lg
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_shr16_ri(REG6,2,DEFAULT);           //lg/4
    VoC_movreg16(REG1,REG7,IN_PARALLEL);        //&y[0]
    VoC_jal(CII_copy_xy);
    VoC_bne16_rd(SYN_FILT_01,REG6,CONST_5_ADDR);
    VoC_sw32_p(ACC0,REG1,DEFAULT);
SYN_FILT_01:
    // Update of memory if update==1
    VoC_lw32_sd(REG01,0,DEFAULT);           //REG1:&mem[0]
    VoC_bez16_r(Syn_filt103,REG0)
    VoC_lw16i(REG0,SYN_FILT_TMP_ADDR);
    VoC_add16_rr(REG0,REG0,REG5,DEFAULT);       //&y[lg-m]
    VoC_jal(CII_Copy_M);
Syn_filt103:
    VoC_pop16(RA,DEFAULT);
    VoC_pop32(REG01,DEFAULT);
    VoC_return;
}



/*************************************************************************
    FUNCTION:  CII_Reorder_lsf()
    Input  REG0(incr 1) ->Word16 *lsf: vector of LSFs   (range: 0<=val<=0.5)  ,
    Output REG0     ->Word16 *lsf: vector of LSFs
    used registers: REG0,REG5,REG4
 *************************************************************************/
void CII_Reorder_lsf (void)

{
    VoC_lw16i(REG5,205);                     //lsf_min = min_dist;
    VoC_loop_i_short(10,DEFAULT);            //for (i = 0; i < n; i++)
    VoC_startloop0                           //{
    VoC_lw16_p(REG4,REG0,DEFAULT);
    VoC_bngt16_rr(Reord_L,REG5,REG4);     //    if (sub (lsf[i], lsf_min) < 0)
    VoC_sw16_p(REG5,REG0,DEFAULT);        //    {     lsf[i] = lsf_min;   move16 (); }
Reord_L:
    VoC_lw16_d(REG4,CONST_205_ADDR);
    VoC_add16inc_rp(REG5,REG4,REG0,DEFAULT);  //    lsf_min = add (lsf[i], min_dist);
    VoC_endloop0
    VoC_return;                              //}
}



/************************************************
  Function CII_Vq_subvec
  Input: RL7 == 0  -->  CII_Vq_subvec,CII_Vq_subvec4
             != 0  -->  CII_Vq_subvec_s
         &lsf_r1[] -->  push32(n+1) LO
         &wf1[]    -->  push32(n+1) HI
         &lsf_r2[] -->  push32(n) LO
         &wf2[]    -->  push32(n) HI
         *dico     -->  REG3(incr=2)
         dico_size/32 -> REG6
         &indice[] -> REG2(incr=1)  push16(n+1)
  requirment:  REG0.incr=2,REG1.incr=2;
  Output:&lsf_r1[+2] -->  push32(n+1) LO
         &wf1[+2]    -->  push32(n+1) HI
         &lsf_r2[+2] -->  push32(n) LO
         &wf2[+2]    -->  push32(n) HI

  Note: CII_Vq_subvec4,CII_Vq_subvec_s,CII_Vq_subvec can use this function only needs to
  confige the interface
  Optimized by Kenneth  20/09/2004
************************************************/

void CII_Vq_subvec(void)
{
    VoC_lw32z(REG45,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x7FFFFFFF_ADDR);
    VoC_push16(REG3,DEFAULT);           //push16(n)  &dico[]
    VoC_lw32_sd(REG01,1,IN_PARALLEL);
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);
    VoC_loop_r(1,REG6);
    VoC_loop_i(0,32);           //in loop REG5 for sign
    VoC_sub16inc_pp(REG6,REG0,REG3,DEFAULT);    //  temp = sub (lsf_r1[0], *p_dico++);
    VoC_sub16inc_pp(REG7,REG0,REG3,IN_PARALLEL);    //  temp = mult (wf1[0], temp);

    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);  //  dist = L_mult (temp, temp);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);  //  temp = sub (lsf_r1[1], *p_dico++);
    VoC_lw32_sd(REG01,0,DEFAULT);                   //  temp = mult (wf1[1], temp);
    VoC_bimac32_rr(REG67,REG67);                    //  dist = L_mac (dist, temp, temp);
    //  temp = sub (lsf_r2[0], *p_dico++);
    VoC_sub16inc_pp(REG6,REG0,REG3,DEFAULT);    //  temp = mult (wf2[0], temp);
    VoC_sub16inc_pp(REG7,REG0,REG3,IN_PARALLEL);    //  dist = L_mac (dist, temp, temp);

    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);  //  temp = sub (lsf_r2[1], *p_dico++);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);  //  temp = mult (wf2[1], temp);
    VoC_NOP();

    VoC_bimac32_rr(REG67,REG67);                    //  dist = L_mac (dist, temp, temp);
    VoC_lw32_sd(REG01,1,DEFAULT);                   //   if (L_sub (dist, dist_min) < (Word32) 0)
    VoC_bngt32_rr(Vq_subvec_s_new_01,RL6,ACC0);     //   {
    VoC_lw16i_short(REG5,0,DEFAULT);        //       dist_min = dist;
    VoC_movreg32(RL6,ACC0,DEFAULT);         //       index = i;
    VoC_movreg16(REG4,REG2,IN_PARALLEL);    //       sign = 0;
Vq_subvec_s_new_01:                                             //    }
    VoC_bez32_r(Vq_subvec_s_new_02,RL7);
    /* test negative */
    VoC_sub16_rd(REG3,REG3,CONST_4_ADDR);            // p_dico -= 4;
    VoC_lw32z(ACC0,DEFAULT);                         // temp = add (lsf_r1[0], *p_dico++);
    VoC_add16inc_pp(REG6,REG0,REG3,DEFAULT);     // temp = mult (wf1[0], temp);
    VoC_add16inc_pp(REG7,REG0,REG3,IN_PARALLEL);     // dist = L_mult (temp, temp);
    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);       // temp = add (lsf_r1[1], *p_dico++);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);   // temp = mult (wf1[1], temp);
    VoC_lw32_sd(REG01,0,DEFAULT);                    // dist = L_mac (dist, temp, temp);
    VoC_bimac32_rr(REG67,REG67);                     // temp = add (lsf_r2[0], *p_dico++);
    // temp = mult (wf2[0], temp);
    VoC_add16inc_pp(REG6,REG0,REG3,DEFAULT);     // dist = L_mac (dist, temp, temp);
    VoC_add16inc_pp(REG7,REG0,REG3,IN_PARALLEL);     // temp = add (lsf_r2[1], *p_dico++);
    VoC_multf16inc_rp(REG6,REG6,REG1,DEFAULT);       // temp = mult (wf2[1], temp);
    VoC_multf16inc_rp(REG7,REG7,REG1,IN_PARALLEL);   // dist = L_mac (dist, temp, temp);
    VoC_NOP();
    VoC_bimac32_rr(REG67,REG67);                     // if (L_sub (dist, dist_min) < (Word32) 0
    VoC_lw32_sd(REG01,1,DEFAULT);                    // {
    //     dist_min = dist;
    VoC_bngt32_rr(Vq_subvec_s_new_02,RL6,ACC0);      //     index = i;
    VoC_lw16i_short(REG5,1,DEFAULT);         //     sign = 1;
    VoC_movreg32(RL6,ACC0,DEFAULT);          // }                                      )
    VoC_movreg16(REG4,REG2,IN_PARALLEL);
Vq_subvec_s_new_02:
    VoC_lw32z(ACC0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1


    VoC_pop16(REG3,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_shr16_ri(REG4,-2,IN_PARALLEL);

    VoC_bez32_r(Vq_subvec_LEnd,RL7);
    VoC_shr16_ri(REG6,-1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
Vq_subvec_LEnd:
    VoC_lw32_sd(ACC0,1,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_lw32_d(RL6,CONST_0x00020002_ADDR);
    VoC_lw32inc_p(REG67,REG3,DEFAULT);
    VoC_lw32_sd(ACC1,0,DEFAULT);//gugogi
    VoC_movreg16(REG0,ACC0_LO,IN_PARALLEL);
    VoC_movreg16(REG1,ACC1_LO,DEFAULT);
    VoC_add32_rr(ACC0,ACC0,RL6,IN_PARALLEL);
    VoC_add32_rr(ACC1,ACC1,RL6,DEFAULT);
    VoC_sw16_sd(REG2,0,IN_PARALLEL);
    VoC_sw32_sd(ACC0,1,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);
    VoC_sw32_sd(ACC1,0,DEFAULT);

    VoC_bnez16_r(Vq_subvec_s_new_05,REG5);      //if (sign == 0)
    VoC_lw32inc_p(REG45,REG3,DEFAULT);      //{
    VoC_sw32_p(REG67,REG0,DEFAULT);                 //lsf_r1[0] = *p_dico++;lsf_r1[1] = *p_dico++;
    VoC_sw32_p(REG45,REG1,DEFAULT);                 //lsf_r2[0] = *p_dico++;lsf_r2[1] = *p_dico++;
    VoC_return ;                                    //}
Vq_subvec_s_new_05:
    VoC_sub16_rr(REG6,REG2,REG6,DEFAULT);       //else{
    VoC_lw32inc_p(REG45,REG3,DEFAULT);              //lsf_r1[0] = negate (*p_dico++);
    VoC_sub16_rr(REG7,REG2,REG7,IN_PARALLEL);       //lsf_r1[1] = negate (*p_dico++);
    VoC_sub16_rr(REG4,REG2,REG4,DEFAULT);           //lsf_r2[0] = negate (*p_dico++);
    VoC_sub16_rr(REG5,REG2,REG5,IN_PARALLEL);       //lsf_r2[1] = negate (*p_dico++);
    VoC_sw32_p(REG67,REG0,DEFAULT);                 //}
    VoC_sw32_p(REG45,REG1,DEFAULT);
    VoC_return;
}


void CII_prm2bits_amr_efr(void)
{
    VoC_lw16i(REG1,BIT_SERIAL_ADDR_RAM);            //REG1:&bits_short
    VoC_lw16i_set_inc(REG2,COD_AMR_ANA_ADDR,1);         //reg2:&ana

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_loop_i(1,57)
    VoC_lw16_p(REG6,REG0,DEFAULT);          //reg6:no_of_bits
    VoC_and16_ri(REG6,0xff);
    VoC_and16_ri(REG4,1);
    VoC_bez16_r(prm2bits_amr_efr_even,REG4)
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_shru16_ri(REG6,8,DEFAULT);
prm2bits_amr_efr_even:

    VoC_lw16inc_p(REG7,REG2,DEFAULT);       //REG7:value
    VoC_add16_rr(REG1,REG1,REG6,DEFAULT);       //bits += bitno[i];
    VoC_sub16_rd(REG3,REG1,CONST_1_ADDR);       //reg3:&bitstream[no_of_bits-1]

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    // added by Yordan 20/09/2006
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_and16_rr(REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);
    VoC_endloop1

    /****************************************************
      make the bit into AMR_122 RING
      done by gugogi
    *****************************************************/

    // extern FILE* file_serial;
    VoC_lw16i_set_inc(REG1,CONST_TAB244_ADDR,2);//temp_table[i]
    VoC_lw16i(REG2,BIT_SERIAL_ADDR_RAM);//short* serialp = &RAM_X[(BIT_SERIAL_ADDR_RAM)/2];
    // char  temp[32];

    VoC_add16inc_rp(REG6,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG7,REG2,REG1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG0,TEMP_MYRING_ADDR,2);//short Myring[248];

    VoC_loop_i(1,122);//for (i=0;i<244;i++)//Myring[i] = serialp[temp_table[i]];


    VoC_lw16inc_p(REG4,REG6,DEFAULT);
    VoC_lw16inc_p(REG5,REG7,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_add16inc_rp(REG6,REG2,REG1,DEFAULT);
    VoC_add16inc_rp(REG7,REG2,REG1,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_endloop1
    ////////////////////////////////////////////////////////
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);//Myring[244] = 0;     Myring[245] = 0;
    VoC_lw16i_short(REG1,-16,IN_PARALLEL);//j

    VoC_lw16i(REG5,0x3c);//temp[0] = 0x3c;
    VoC_lw16i_set_inc(REG0,TEMP_MYRING_ADDR,2);//short Myring[248];
    exit_on_warnings = OFF;
    VoC_sw32_p(ACC0,REG0,DEFAULT);//Myring[246] = 0;    Myring[247] = 0;
    VoC_lw16i_short(INC1,1,IN_PARALLEL);
    exit_on_warnings = ON;
    //////////////////////////////////////////////////////
    /******************************************
    *  244/8=31 char  + "0x3c" = 32 char = 16 short
    *******************************************/
//   VoC_lw16d_set_inc(REG2,ENC_OUTPUT_ADDR_ADDR,1);// 4 + OUTPUT_ADDR
    //VoC_add16_rd(REG2,REG2,CONST_4_ADDR);//out put addr


    VoC_lw16i_set_inc(REG2,OUTPUT_BITS_ADDR,1);

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i++)
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);//temp[j]|=(Myring[(j-1)*8+i]<<(7-i));
    VoC_startloop0

    VoC_inc_p(REG1,DEFAULT);
    VoC_shru16_rr(REG6,REG1,IN_PARALLEL);//temp[j]|=(Myring[(j-1)*8+i]<<(7-i));

    VoC_or16_rr(REG4,REG6,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_shru16_rr(REG7,REG1,DEFAULT);

    VoC_or16_rr(REG4,REG7,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);

    VoC_endloop0


    VoC_or16_rr(REG5,REG4,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);//DMAE  L_ADDR
    VoC_lw16i_short(REG1,-8,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    //////////////the remain 15 short//////////////////////


    VoC_loop_i(1,15);//for (j=1;j<32;j++)

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i++)   one char
    VoC_lw32z(REG45,IN_PARALLEL);
    VoC_startloop0

    VoC_inc_p(REG1,DEFAULT);
    VoC_shru16_rr(REG6,REG1,IN_PARALLEL);//temp[j]|=(Myring[(j-1)*8+i]<<(7-i));

    VoC_or16_rr(REG5,REG6,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_shru16_rr(REG7,REG1,DEFAULT);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);
    VoC_endloop0
    ///////////////////////////////

    VoC_lw16i_short(REG1,-16,DEFAULT);

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i++)   //another char
    VoC_startloop0

    VoC_inc_p(REG1,DEFAULT);
    VoC_shru16_rr(REG6,REG1,IN_PARALLEL);//temp[j]|=(Myring[(j-1)*8+i]<<(7-i));

    VoC_or16_rr(REG4,REG6,DEFAULT);

    VoC_inc_p(REG1,DEFAULT);
    VoC_shru16_rr(REG7,REG1,IN_PARALLEL);


    VoC_or16_rr(REG4,REG7,DEFAULT);
    VoC_lw32inc_p(REG67,REG0,IN_PARALLEL);

    VoC_endloop0
    ///////////////////////////

    // VoC_shru16_ri(REG4,-8,DEFAULT);
    VoC_or16_rr(REG5,REG4,DEFAULT);//one short
    VoC_lw16i_short(REG1,-8,DEFAULT);
    VoC_sw16inc_p(REG5,REG2,DEFAULT);//save this short

    VoC_endloop1


    VoC_pop16(REG3,DEFAULT);//DMAE  L_ADDR
    VoC_shru16_ri(REG3,1,DEFAULT);//[INT]

//     //DMAI WRITE////////////////////////////
//     VoC_lw16i_short(REG2,8,IN_PARALLEL);
//       VoC_lw32_d(ACC1,GLOBAL_AMR_EXOUTPUT_ADDR);
//
//     VoC_sw32_d(REG23,CFG_DMA_SIZE);
//     VoC_sw32_d(ACC1,CFG_DMA_EADDR);
//
//      VoC_cfg(CFG_CTRL_STALL);
//      VoC_NOP();
//      VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    ////////////////////////////////////////////////
    VoC_return;
}
/////////////////////////////////

void CII_dtx_buffer(void)
{
    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_add16_rd(REG6,REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_lw16i_set_inc(REG2,STRUCT_COD_AMRSTATE_NEW_SPEECH_ADDR,2);
    VoC_bne16_rd(NOT_FULL,REG6,CONST_8_ADDR)
    VoC_lw16i_short(REG6,0,DEFAULT);
NOT_FULL:
    VoC_lw16i_short(REG7,10,DEFAULT);

    VoC_mult16_rr(REG7,REG7,REG6,DEFAULT);
    VoC_lw16i_set_inc(REG0,COD_AMR_LSP_NEW_ADDRESS,2);
    VoC_lw16i_set_inc(REG1,STRUCT_DTX_ENCSTATE_LSP_HIST_ADDR,2);

    VoC_sw16_d(REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_jal(CII_dtx_com_s2);

    VoC_lw16i(REG6,STRUCT_DTX_ENCSTATE_LOG_EN_HIST_ADDR);
    VoC_add16_rd(REG6,REG6,STRUCT_DTX_ENCSTATE_HIST_PTR_ADDR);
    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_return;
}







void CII_PRM2_BITS_POST_PROCESS(void)
{
    VoC_pop16(REG3,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);
//   Copy(&st->old_speech[L_FRAME], &st->old_speech[0], L_TOTAL - L_FRAME);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_SPEECH_ADDR,2);

    VoC_jal(CII_copy_xy);

//  VoC_lw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR+160,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR,1);
//  VoC_sw16_d(REG5, STRUCT_COD_AMRSTATE_OLD_WSP_ADDR);

    VoC_lw16inc_p(REG6,REG0,DEFAULT);

    exit_on_warnings = OFF;
    VoC_loop_i(0,143)
    VoC_lw16inc_p(REG6,REG0,DEFAULT);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_endloop0

    exit_on_warnings = ON;
    //begin of CII_prm2bits

    VoC_lw16i_set_inc(REG0,STATIC_CONST_BITNO_ADDR,1);

    VoC_jal(CII_prm2bits_amr_efr);


    VoC_lw16i_short(REG6,0,DEFAULT);    //  *tx_frame_type = TX_SPEECH_GOOD;
    VoC_pop16(RA,IN_PARALLEL);

    VoC_NOP();
//  VoC_lw32_d(REG45,STRUCT_SID_SYNCSTATE_SID_UPDATE_RATE_ADDR);

//    VoC_sw16_d(REG6,STRUCT_SID_SYNCSTATE_PREV_FT_ADDR);       // st->prev_ft = *tx_frame_type;
//  VoC_sw16_d(REG4,STRUCT_SID_SYNCSTATE_SID_UPDATE_COUNTER_ADDR);
    // REG6 for frame_type
    VoC_return;
}




/***************************************************************************
 *                                                                          *
 *  Function:  check_lsp()                                                  *
 *  Purpose:   Check the LSP's to detect resonances                         *
 *                                                                          *
 ****************************************************************************
 */

void CII_check_lsp(void)
{


    VoC_lw16i_short(REG5,2,DEFAULT);
    VoC_lw16i_set_inc(REG0,STRUCT_LSPSTATE_LSP_OLD_ADDR+1,1);
    VoC_loop_i(1, 2)
    VoC_lw16i(REG4,MAX_16);                                 //dist_min2
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG0,DEFAULT);
    VoC_sub16_rp(REG5,REG5,REG0,DEFAULT);
    VoC_bngt16_rr(CHECK_LSP_02,REG4,REG5)
    VoC_movreg16(REG4,REG5,DEFAULT);
CHECK_LSP_02:
    VoC_lw16i_short(REG5,5,DEFAULT);
    VoC_endloop0
    VoC_movreg16(REG7, REG6, DEFAULT);
    VoC_movreg16(REG6, REG4, IN_PARALLEL);
//  VoC_lw16i(REG4,MAX_16);
    VoC_endloop1

//REG7   dist_min2    REG6    dist_min1

    VoC_lw16i(REG4,32000);
    VoC_lw16i(REG5,600);            //dist_th

    VoC_blt16_rd(CHECK_LSP_03,REG4,STRUCT_LSPSTATE_LSP_OLD_ADDR+1)
    VoC_lw16i(REG4,30500);
    VoC_lw16i(REG5,800);
    VoC_blt16_rd(CHECK_LSP_03,REG4,STRUCT_LSPSTATE_LSP_OLD_ADDR+1)
    VoC_lw16i(REG5,1100);
CHECK_LSP_03:
//REG5    dist_th

    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG1,0,DEFAULT);//st->count
    VoC_lw16i(REG4,1500);
    VoC_bgt16_rr(CHECK_LSP_04,REG4,REG6)
    VoC_bngt16_rr(CHECK_LSP_05,REG5,REG7)
CHECK_LSP_04:
    VoC_add16_rd(REG1,REG0,STRUCT_TONSTABSTATE_COUNT_ADDR);
CHECK_LSP_05:

    /* Need 12 consecutive frames to set the flag */
    VoC_lw16i_short(REG7,12,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);    //return value

    VoC_bgt16_rr(CHECK_LSP_RET,REG7,REG1)
    VoC_movreg16(REG1,REG7,DEFAULT);
    VoC_lw16i_short(REG6,1,DEFAULT);

CHECK_LSP_RET:
    VoC_sw16_d(REG1,STRUCT_TONSTABSTATE_COUNT_ADDR);
    VoC_sw16_d(REG6,COD_AMR_LSP_FLAG_ADDRESS);

    VoC_return

}



/***********************************************
 Function : CII_dtx_com
 Input: reg0(incr=1)    :&predState->past_qua_en[]
        reg7        :log_en(ma_pred_init)
 Note: dtx_dec and dtx_enc share the same code
 Optimized by Kenneth  22/09/2004
***********************************************/


void CII_RESET_AMR_CONSTANT(void)
{
    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_set_inc(REG0,PRAM4_TABLE_ADDR,2);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,7,IN_PARALLEL);
    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG4,6,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);
    VoC_lw16i_short(REG6,12,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_sw16_p(REG3,REG0,DEFAULT);
    VoC_return;
}
///////////////////////////////////////////////////
void CII_COD_AMR_122(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_push16(REG7,DEFAULT); // COD_AMR_ANAP_ADDR
    VoC_jal(CII_lpc_M122);



    VoC_jal(CII_lsp_122);
    /* From A(z) to lsp. LSP quantization and interpolation */
//   lsp(st->lspSt, mode, *usedMode, A_t, Aq_t, lsp_new, &ana);
    /* Buffer lsp's and energy */
    /*
    dtx_buffer(st->dtx_encSt,
          lsp_new,
          st->new_speech);
    */

//////////////////////////

//  VoC_jal(CII_dtx_buffer);

    /* check resonance in the filter */
//      lsp_flag = check_lsp(st->tonStabSt, st->lspSt->lsp_old);  move16 ();
    VoC_jal(CII_check_lsp);


//cod_amr103:

    VoC_lw32z(REG67,DEFAULT);     // REG7 for i_subfr // REG6 for subfrnr

    VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS,1);
    VoC_lw16i_set_inc(REG3,STRUCT_COD_AMRSTATE_SPEECH_ADDR,1);
    VoC_lw16i_set_inc(REG2,STRUCT_COD_AMRSTATE_WSP_ADDR,1);

cod_amr105:

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_sw32_d(REG67,COD_AMR_SUBFRNR_ADDRESS);
    VoC_blt16_rd(cod_amr1041,REG6,CONST_4_ADDR)
    VoC_jump(cod_amr104);
cod_amr1041:/*
      pre_big(mode, gamma1, gamma1_12k2, gamma2, A_t, i_subfr, st->speech,
              st->mem_w, st->wsp);
*/
    VoC_lw16i_short(REG4,5,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma1_12K2_ADDR,1);
//PRE_BIG01:

    VoC_lw16i_set_inc(REG2,PRE_BIG_AP1_ADDR,1);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,STATIC_CONST_gamma2_ADDR,1);
    VoC_lw16i_set_inc(REG2,PRE_BIG_AP2_ADDR,1);
    VoC_jal(CII_Weight_Ai);
    VoC_lw16i_set_inc(REG0,PRE_BIG_AP1_ADDR,1);
    VoC_lw32_sd(REG23,0,DEFAULT);
    VoC_jal(CII_Residu_new);

    VoC_lw32_sd(REG23,0,DEFAULT);

    VoC_lw16i(REG5,PRE_BIG_AP2_ADDR);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_lw16i_short(REG6,40,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_MEM_W_ADDR,1);
    VoC_movreg16(REG3,REG2,DEFAULT);
    VoC_movreg16(REG7,REG2,IN_PARALLEL);
    /*__________________________________________________________________
      Function:  CII_Syn_filt
     prameters:
       &a[0]  ->reg5
       &x[0]  -> reg3(inc 1 )
       &y     ->REG7
       lg     ->REG6
       &mem[0]->REG1 (incr=1) push32:
       update ->reg0

     Version 1.0  Create
     Version 1.1 optimized by Kenneth 09/01/2004
    ____________________________________________________________________*/
    VoC_jal(CII_Syn_filt);

//////////////////////////////////////////
    VoC_lw16_d(REG6,COD_AMR_SUBFRNR_ADDRESS);
    VoC_be16_rd(cod_amr106,REG6,CONST_0_ADDR)
    VoC_be16_rd(cod_amr106,REG6,CONST_2_ADDR)
    /* Find open loop pitch lag for two subframes */
    VoC_jal(CII_Pitch_ol_M122);

    /*
             ol_ltp(st->pitchOLWghtSt, st->vadSt, mode, &st->wsp[i_subfr],
                    &T_op[subfrNr], st->old_lags, st->ol_gain_flg, subfrNr,
                    st->dtx);
    */
cod_amr106:
    VoC_lw16i(REG6,COD_AMR_T_OP_ADDRESS);
    VoC_lw16_d(REG7,COD_AMR_SUBFRNR_ADDRESS);
    VoC_shr16_ri(REG7,1,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG7,DEFAULT);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(REG23,IN_PARALLEL);
    VoC_sw16_p(REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG4,11,DEFAULT);
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
    VoC_add16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);
    VoC_lw32_d(REG67,COD_AMR_SUBFRNR_ADDRESS);
    VoC_add16_rd(REG6,REG6,CONST_1_ADDR);
    VoC_bne16_rd(cod_amr601,REG6,CONST_3_ADDR)
    VoC_add16_rd(REG7,REG7,CONST_80_ADDR);
cod_amr601:
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

//  VoC_sw32_d(REG67,COD_AMR_SUBFRNR_ADDRESS);

    VoC_jump(cod_amr105);
cod_amr104:
    /* run VAD pitch detection */


    /*------------------------------------------------------------------------*
    *          Loop for every subframe in the analysis frame                 *
    *------------------------------------------------------------------------*
    *  To find the pitch and innovation parameters. The subframe size is     *
    *  L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.               *
    *     - find the weighted LPC coefficients                               *
    *     - find the LPC residual signal res[]                               *
    *     - compute the target signal for pitch search                       *
    *     - compute impulse response of weighted synthesis filter (h1[])     *
    *     - find the closed-loop pitch parameters                            *
    *     - encode the pitch dealy                                           *
    *     - update the impulse response h1[] by including fixed-gain pitch   *
    *     - find target vector for codebook search                           *
    *     - codebook search                                                  *
    *     - encode codebook address                                          *
    *     - VQ of pitch and codebook gains                                   *
    *     - find synthesis speech                                            *
    *     - update states of weighting filter                                *
    *------------------------------------------------------------------------*/

    VoC_lw16i_short(REG7,0,DEFAULT);    // REG7 for loop i_subfr
    VoC_lw16i_short(REG6,1,DEFAULT);    // REG6 for loop evenSubfr
//  VoC_lw16i_short(REG5,0,IN_PARALLEL);    // REG5 for loop subfrNr
    VoC_lw16i(REG0,COD_AMR_A_T_ADDRESS);    // REG0 for A
    VoC_lw16i(REG1,COD_AMR_AQ_T_ADDRESS);   // REG1 for Aq
    VoC_push16(REG7,DEFAULT);
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG6,DEFAULT);
    VoC_push16(REG7,DEFAULT);
cod_amr112:
    VoC_blt16_rd(cod_amr1111,REG7,CONST_160_ADDR)
    /* Save states for the MR475 mode */
    VoC_jump(cod_amr111);
cod_amr1111:

    /*-----------------------------------------------------------------*
       * - Preprocessing of subframe                                     *
       *-----------------------------------------------------------------*/
    /*         subframePreProc(*usedMode, gamma1, gamma1_12k2,
                             gamma2, A, Aq, &st->speech[i_subfr],
                             st->mem_err, st->mem_w0, st->zero,
                             st->ai_zero, &st->exc[i_subfr],
                             st->h1, xn, res, st->error);
    */
    VoC_lw16i(RL7_HI,STRUCT_COD_AMRSTATE_MEM_W0_ADDR);
    VoC_jal(CII_subframePreProc);
    /* copy the LP residual (res2 is modified in the CL LTP search)    */
    // Copy (res, res2, L_SUBFR);

    VoC_lw16_sd(REG7,0,DEFAULT);
    VoC_lw16i(REG5,STRUCT_COD_AMRSTATE_EXC_ADDR);
    VoC_add16_rr(REG7,REG7,REG5,DEFAULT);

    VoC_lw16i_short(REG5,0,DEFAULT);    // REG5 for delta_search=1;
    /*-----------------------------------------------------------------*
     * - Closed-loop LTP search                                        *
     *-----------------------------------------------------------------*/
    /*      cl_ltp(st->clLtpSt, st->tonStabSt, *usedMode, i_subfr, T_op, st->h1,
                 &st->exc[i_subfr], res2, xn, lsp_flag, xn2, y1,
                 &T0, &T0_frac, &gain_pit, gCoeff, &ana,
                 &gp_limit);*/

    VoC_sw16_d(REG7,COD_AMR_EXC_ADDR_ADDRESS);

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//  begin of  CII_cl_ltp
    //*T0 = Pitch_fr(clSt->pitchSt,mode, T_op, exc, xn, h1, L_SUBFR, frameOffset,T0_frac, &resu3, &index);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //  begin of  CII_Pitch_fr

    VoC_lw16_sd(REG7,0,DEFAULT);        // REG7 for i_subfr
    VoC_lw16i_short(REG1,18,IN_PARALLEL);   // REG1 for pit_min

    VoC_lw16_d(REG0,COD_AMR_T_OP_ADDRESS);
//  VoC_lw32_d(REG23,STATIC_CONST_mode_dep_parm_MR122_ADDR+4);
    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_lw16i_short(REG3,6,IN_PARALLEL);


    VoC_bez16_r(AMR_PITCH_FR201,REG7)

    VoC_lw16_d(REG0,COD_AMR_T_OP_ADDRESS+1);

    VoC_be16_rd(AMR_PITCH_FR201,REG7,CONST_80_ADDR)
    VoC_lw16i_short(REG5,1,DEFAULT);    // REG5 for delta_search=0;
    VoC_lw16_d(REG0,STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR);
//  VoC_lw32_d(REG23,STATIC_CONST_mode_dep_parm_MR122_ADDR+6);

    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16i_short(REG3,9,IN_PARALLEL);
AMR_PITCH_FR201:
    VoC_sub16_rr(REG4,REG0,REG2,DEFAULT);   //      *t0_min = sub(T0, delta_low);
    VoC_sw16_d(REG5,Pitch_delta_search_addr);// save delta_search
    /* REG0 for T0;REG23 for delta_frc_low,delta_frc_range; REG1 for pit_min; REG6 for PIT_MAX; REG45 for t0_min,t0_max */
    /*++++++++++++++++++++++++++++++++++++++++++++++++*/
    // begin of CII_getRange
    VoC_bgt16_rr(GET_RANGE1,REG4,REG1)      //      if (sub(*t0_min, pitmin) < 0) {
    VoC_movreg16(REG4,REG1,DEFAULT);    //      *t0_min = pitmin;                                  move16();
GET_RANGE1:
    VoC_add16_rr(REG5,REG4,REG3,DEFAULT);   //      *t0_max = add(*t0_min, delta_range);
    VoC_bngt16_rd(GET_RANGE2,REG5,CONST_143_ADDR)       //      if (sub(*t0_max, pitmax) > 0) {
    VoC_lw16i(REG5,143);                //      *t0_max = pitmax;                                  move16();
    VoC_sub16_rr(REG4,REG5,REG3,DEFAULT);   //      *t0_min = sub(*t0_max, delta_range);
GET_RANGE2:
    // end of CII_getRange
    /*++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*-----------------------------------------------------------------------*
     *           Find interval to compute normalized correlation             *
     *-----------------------------------------------------------------------*/

    VoC_lw16i_set_inc(REG0,Pitch_fr6_corr_v_addr,2);
    VoC_sw32_d(REG45,Pitch_fr6_t0_min_addr);
    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);           //t_min = sub (t0_min, L_INTER_SRCH);
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);       //t_max = add (t0_max, L_INTER_SRCH);

    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_H1_ADDR,-2); /*REG1=h[n]地址*/
    VoC_sub16_rr(REG2,REG0,REG4,DEFAULT);           //REG2 for address &corr_v[-t_min];
    VoC_push32(REG45,IN_PARALLEL);      // stack32 0 for t_min t_max
//    Norm_Corr (exc, xn, h, L_subfr, t_min, t_max, corr);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    // begin of CII_Norm_Corr

    VoC_sub16_dr(REG0,COD_AMR_EXC_ADDR_ADDRESS,REG4);   //k = -t_min;
    VoC_lw16i_set_inc(REG3,PITCH_F6_EXCF_ADDR,1);
    VoC_push16(REG2,DEFAULT);   // stack16 1 for corr_v[-t_min] address
    VoC_push16(REG0,DEFAULT);   // stack16 0 for exc[k] address
//    Convolve (&exc[k], h, excf, L_subfr);
    VoC_jal(CII_Convolve);
    VoC_lw32z(ACC0,DEFAULT);  //  s = 0;
    VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);

    VoC_loop_i_short(20,DEFAULT)  //  for (j = 0; j < L_subfr; j++)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);   // s = L_mac (s, excf[j], excf[j]);
    VoC_endloop0

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i(REG3,0x0400);
    VoC_lw16i_short(REG4,-3,DEFAULT);                               //-h_fac
    VoC_lw16i_short(REG5,0,IN_PARALLEL);        // scaling
    VoC_bngt32_rr(PITCH_F6_1001,ACC0,REG23);

    /* scale "excf[]" to avoid overflow */
    VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);           /* REG0 for excf[i] address */
    VoC_lw16i_set_inc(REG1,PITCH_F6_EXCF_ADDR,2);           /* REG1 for s_excf[i] address */
    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_lw16i_short(REG3,10,IN_PARALLEL);
    VoC_jal(CII_scale);

    VoC_lw16i_short(REG4,-1,DEFAULT);                           //-h_fac
    VoC_lw16i_short(REG5,2,DEFAULT);
PITCH_F6_1001:

//    for (j = 0; j < L_subfr; j++)
//        scaled_excf[j] = shr (excf[j], 2);
    VoC_lw16_sd(REG2,1,DEFAULT);        //REG2 for address &corr_v[-t_min];
    VoC_lw32_sd(REG67,0,DEFAULT);   // REG6 for t_min, REG7 for t_max
    VoC_add16_rr(REG3,REG2,REG7,DEFAULT);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_sw16_d(REG3,Pitch_fr6_max_loop_addr);       // for decide the last loop
    VoC_add16_rr(REG2,REG2,REG6,DEFAULT);       // REG2 for corr_norm[i] address
    VoC_lw16_sd(REG3,0,IN_PARALLEL);            // REG3 for exc[k] address
PITCH_F6_1002:  // begin loop
    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);

    VoC_loop_i_short(20,DEFAULT)
    VoC_push32(REG45,IN_PARALLEL);  // -h_fac scaling
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0
    VoC_push32(REG23,DEFAULT);      // save exc corr_v address
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_jal(CII_Inv_sqrt);
    VoC_movreg32(REG67,REG01,DEFAULT);

    /* Compute correlation between xn[] and excf[] */
    VoC_lw16i_set_inc(REG0,PITCH_F6_EXCF_ADDR,2);
    VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2);
    VoC_loop_i_short(20,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_shru16_ri(REG6,1,DEFAULT);
//  VoC_and16_ri(REG6,0x7FFF);              //nor_l
    VoC_movreg32(REG45,ACC0,DEFAULT);       //corr_h
    VoC_shru16_ri(REG4,1,DEFAULT);
//  VoC_and16_ri(REG4,0x7FFF);              //corr_l
//        s = Mpy_32 (corr_h, corr_l, norm_h, norm_l);
    VoC_multf32_rr(ACC0,REG7,REG5,DEFAULT);
    VoC_multf16_rr(REG0,REG7,REG4,IN_PARALLEL);

    VoC_multf16_rr(REG1,REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_mac32_rr(REG0,REG3,DEFAULT);
    VoC_mac32_rr(REG1,REG3,DEFAULT);
    VoC_pop32(REG23,DEFAULT);       // REG3 for exc[k]
    VoC_shr32_ri(ACC0,-16,DEFAULT);

    VoC_lw16i_short(INC3,-1,DEFAULT);
//        corr_norm[i] = extract_h (L_shl (s, 16));
    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);
    VoC_pop32(REG45,DEFAULT);   // -h_fac scaling

    VoC_bgt16_rd(PITCH_F6_1003,REG2,Pitch_fr6_max_loop_addr)
    VoC_inc_p(REG3,DEFAULT);      //      k--;
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_H1_ADDR+39,-1);
    VoC_lw16i_set_inc(REG1,PITCH_F6_EXCF_ADDR+38,1);
    VoC_lw16_p(REG6,REG3,DEFAULT);  // REG6 for exc[k]
    VoC_push32(REG23,DEFAULT);
    VoC_multf32inc_rp(REG23,REG6,REG0,DEFAULT);   //   s = L_mult (exc[k], h[j]);

    VoC_loop_i_short(39,DEFAULT)  //   for (j = L_subfr - 1; j > 0; j--)
    VoC_startloop0
    VoC_shr32_rr(REG23,REG4,DEFAULT);       //  s = L_shl (s, h_fac);
    VoC_add16inc_rp(REG3,REG3,REG1,DEFAULT);
    VoC_multf32inc_rp(REG23,REG6,REG0,DEFAULT);   //   s = L_mult (exc[k], h[j]);
    exit_on_warnings = OFF;
    VoC_sw16_p(REG3,REG1,DEFAULT);    //  s_excf[j] = add (extract_h (s), s_excf[j - 1]);
    exit_on_warnings = ON;
    VoC_sub16_rd(REG1,REG1,CONST_2_ADDR);
    VoC_endloop0

    VoC_shr16_rr(REG6,REG5,DEFAULT);
    VoC_pop32(REG23,DEFAULT);
    VoC_sw16_d(REG6,PITCH_F6_EXCF_ADDR);     //  s_excf[0] = shr (exc[k], scaling);
    VoC_jump(PITCH_F6_1002);
PITCH_F6_1003:
    // end of CII_Norm_Corr
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_pop32(REG45,DEFAULT);
    VoC_pop16(REG2,IN_PARALLEL);
    /*-----------------------------------------------------------------------*
     *                           Find integer pitch                          *
     *-----------------------------------------------------------------------*/

    VoC_lw16_sd(REG0,0,DEFAULT);    //  corr[]

    VoC_lw32_d(REG45,Pitch_fr6_t0_min_addr);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_movreg16(REG1,REG4,DEFAULT);            //REG1 for lag   lag = t0_min;
    VoC_lw16inc_p(REG7,REG0,DEFAULT);           //REG2 for max
    VoC_sub16_rr(REG6,REG5,REG4,IN_PARALLEL);       //REG6 for loop number
    VoC_movreg16(REG2,REG7,DEFAULT);            //  max = corr[t0_min];
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_bngtz16_r(AMR_PITCH_FR103,REG6)

    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_loop_r_short(REG6,DEFAULT);     //  for (i = t0_min + 1; i <= t0_max; i++)
    VoC_startloop0
    VoC_sub16_rr(REG3,REG7,REG2,DEFAULT);
    VoC_bltz16_r(AMR_PITCH_FR104,REG3)          //   if (sub (corr[i], max) >= 0) {

    VoC_movreg16(REG2,REG7,DEFAULT);        // max = corr[i];
    VoC_movreg16(REG1,REG4,DEFAULT);    //     lag = i;
AMR_PITCH_FR104:
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    VoC_endloop0;
AMR_PITCH_FR103:
    /*-----------------------------------------------------------------------*
     *                        Find fractional pitch                          *
     *-----------------------------------------------------------------------*/
    VoC_lw16i_short(RL6_LO,0,DEFAULT);  // frac = 0;        // RL6_LO for frac RL6_HI for last_frac
//  VoC_lw16_d(REG5,Pitch_delta_search_addr);   // REG5 for delta_search
//  VoC_lw16_d(REG4,STATIC_CONST_mode_dep_parm_MR122_ADDR); // REG4 for max_frac_lag
    VoC_lw16i_short(REG4,0x5e,IN_PARALLEL);
    VoC_bnez16_d(AMR_PITCH_FR105,Pitch_delta_search_addr)
//  VoC_bnez16_r(AMR_PITCH_FR105,REG5)
    VoC_bgt16_rr(AMR_PITCH_FR106,REG1,REG4)     // if ((delta_search == 0) && (sub (lag, max_frac_lag) > 0))
AMR_PITCH_FR105:

    VoC_lw16i_short(INC2,-1,DEFAULT);
//  VoC_lw32_d(RL6,STATIC_CONST_mode_dep_parm_MR122_ADDR+2);
//  VoC_lw32_d(REG45,STATIC_CONST_mode_dep_parm_MR122_ADDR+2);
    VoC_lw16i_short(REG4,0xfffd,DEFAULT);
    VoC_lw16i_short(REG5,3,IN_PARALLEL);


//             searchFrac (&lag, &frac, last_frac, corr, flag3);
//          searchFrac (&lag, &frac, last_frac, corr, flag3);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    //   begin of CII_searchFrac
    VoC_lw16_sd(REG2,0,DEFAULT);
//  VoC_movreg32(REG45,RL6,IN_PARALLEL);    // REG4 for frac REG5 for last_frac
    VoC_add16_rr(REG2,REG1,REG2,DEFAULT);   //REG2 for corr[*lag] address    x1 = &x[0];
    VoC_push16(REG1,IN_PARALLEL);       //  REG1 ->lag

    VoC_lw16i_set_inc(REG3,STATIC_CONST_INTER_6_25_ADDR,1);     //&inter_6[0]

    VoC_lw16i(RL6_HI,0x8000);       // RL6_HI for max

    VoC_lw16i_short(INC0,6,DEFAULT);
    VoC_lw16i_short(INC1,6,IN_PARALLEL);
SEARCHFRAC102:

    VoC_bgt16_rr(SEARCHFRAC100,REG4,REG5)   //for (i = add (*frac); i <= last_frac; i++)
    VoC_movreg16(REG6,REG4,DEFAULT);
    VoC_push32(REG23,IN_PARALLEL);
// corr_int = Interpol_3or6 (&corr[*lag], i, flag3);
//    begin of Interpol_3or6
    VoC_bnltz16_r(INTERPOL_3OR6_101,REG6)  //  if (frac < 0)
    VoC_add16_rd(REG6,REG6,CONST_6_ADDR);   //  frac = add (frac, UP_SAMP_MAX);
    VoC_sub16_rd(REG2,REG2,CONST_1_ADDR);   // x--;
INTERPOL_3OR6_101:
    VoC_add16_rr(REG0,REG3,REG6,DEFAULT);       // c1 = &inter_6[frac];
    VoC_sub16_rr(REG1,REG3,REG6,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_6_ADDR);       // c2 = &inter_6[sub (UP_SAMP_MAX, frac)];
    VoC_add16_rd(REG3,REG2,CONST_1_ADDR);   // x2 = &x[1];
    VoC_loop_i_short(4,DEFAULT)
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32inc_pp(REG0,REG2,DEFAULT); //  s = L_mac (s, x1[-i], c1[k]);
    VoC_mac32inc_pp(REG1,REG3,DEFAULT); //  s = L_mac (s, x2[i], c2[k]);
    VoC_endloop0
//    end of Interpol_3or6
    VoC_movreg16(REG0,RL6_HI,DEFAULT);
    VoC_add32_rd(REG23,ACC0,CONST_0x00008000_ADDR);
    VoC_bngt16_rr(SEARCHFRAC101,REG3,REG0);        // if (sub (corr_int, max) > 0)
    VoC_movreg16(RL6_LO,REG4,DEFAULT);          // *frac = i;
    VoC_movreg16(RL6_HI,REG3,IN_PARALLEL);          // max = corr_int;
SEARCHFRAC101:
    VoC_pop32(REG23,DEFAULT);
    VoC_add16_rd(REG4,REG4,CONST_1_ADDR);       //i++
    VoC_jump(SEARCHFRAC102);
SEARCHFRAC100:


    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_pop16(REG1,DEFAULT);
    VoC_movreg16(REG0,RL6_LO,IN_PARALLEL);  // laod frac

    VoC_bne16_rd(SEARCHFRAC104,REG0,CONST_neg3_ADDR);    // if (sub (*frac, -3) == 0)
    VoC_lw16i_short(REG0,3,DEFAULT);    // *frac = 3;
    VoC_sub16_rr(REG1,REG1,REG4,IN_PARALLEL);// *lag = sub (*lag, 1);
SEARCHFRAC104:
    VoC_movreg16(RL6_LO,REG0,DEFAULT);
    //   end of CII_searchFrac
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
AMR_PITCH_FR106:
    /* REG1 for lag */
    VoC_pop16(REG2,DEFAULT);
//  VoC_jal(CII_Enc_lag6);


    VoC_sub16_rd(REG3,REG1,Pitch_fr6_t0_min_addr);  // REG3 for i  i = sub (T0, T0_min);
    VoC_mult16_rd(REG3,REG3,CONST_6_ADDR);
    VoC_movreg16(REG7,RL6_LO,DEFAULT);      // REG7 for t0_frac
    VoC_add16_rd(REG3,REG3,CONST_3_ADDR);
    VoC_add16_rr(REG2,REG3,REG7,DEFAULT);           // index = add (add (add (i, i), 3), T0_frac);

    VoC_bnez16_d(ENC_LAG6_100,Pitch_delta_search_addr)

    VoC_lw16i(REG6,368);
    VoC_add16_rr(REG2,REG1,REG6,DEFAULT);       // index = add (T0, 368);

    VoC_lw16i(REG6,94);
    VoC_bgt16_rr(ENC_LAG6_100,REG1,REG6)        //   if (sub (T0, 94) <= 0)
    VoC_add16_rr(REG3,REG1,REG1,DEFAULT);       // REG3 for i
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);       // i = add (add (T0, T0), T0);
    VoC_lw16i(REG6,105);
    VoC_add16_rr(REG3,REG3,REG3,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG3,REG7,DEFAULT);       // index = add (sub (add (i, i), 105), T0_frac);


ENC_LAG6_100:

    VoC_lw16_sd(REG7,5,DEFAULT);//COD_AMR_ANAP_ADDR
    VoC_sw16_d(REG1,STRUCT_PITCH_FRSTATE_T0_PREV_SUBFRAME_ADDR);    //st->T0_prev_subframe = lag;
    VoC_sw16_p(REG2,REG7,DEFAULT);
    VoC_lw16i_short(REG2,1,DEFAULT);    //REG2 for flag3
    VoC_add16_rr(REG7,REG7,REG2,DEFAULT);

    VoC_sw16_d(REG1,COD_AMR_T0_ADDRESS);
    VoC_sw16_sd(REG7,5,DEFAULT);// COD_AMR_ANAP_ADDR

    VoC_sw16_d(RL6_LO,COD_AMR_T0_FRAC_ADDRESS);
//   *(*anap)++ = index;

    //  end of CII_Pitch_fr
    /*++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*---------------------------------------------------------------------------
    Function:  void CII_Pred_lt_3or6(void)
    Word16 exc[],        in/out: REG5
    Word16 T0,           input : REG6
    Word16 frac,         input : REG7
    Word16 L_subfr,      input : subframe size
    Word16 flag3         input : REG2
    -----------------------------------------------------------------------------*/
    VoC_lw16_d(REG5,COD_AMR_EXC_ADDR_ADDRESS);
    VoC_lw32_d(REG67,COD_AMR_T0_ADDRESS);//get T0 addr REG6  get frac in REG7
    VoC_lw16i_short(REG2,0,DEFAULT);    // flag3 = 0
    VoC_jal(CII_Pred_lt_3or6);
    /* Convolve(exc, h1, y1, L_SUBFR); */
    VoC_lw16d_set_inc(REG0,COD_AMR_EXC_ADDR_ADDRESS,2);
    VoC_lw16i_set_inc(REG3,COD_AMR_Y1_ADDRESS,1);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_H1_ADDR,-2); /*REG7=h[n]地址*/
    VoC_jal(CII_Convolve);
    /* gain_pit is Q14 for all modes
    *gain_pit = G_pitch(mode, xn, y1, g_coeff, L_SUBFR); */
    VoC_jal(CII_G_pitch);
    //  REG2 for gain_pit
    /* check if the pitch gain should be limit due to resonance in LPC filter */
    VoC_lw16_d(REG0,COD_AMR_LSP_FLAG_ADDRESS);
    VoC_lw16i_short(REG7,0,DEFAULT);        // REG7 for  gpc_flag = 0;
    VoC_movreg16(RL7_LO,REG2,IN_PARALLEL);  // RL7_LO->gain_pit
    VoC_lw16i(RL7_HI,0x7FFF);       // RL7_HI->gp_limit                 // REG6 for *gp_limit = MAX_16;
//  VoC_bnez16_d(cl_ltp1202,GLOBAL_EFR_FLAG_ADDR)
    VoC_bez16_d(cl_ltp101,COD_AMR_LSP_FLAG_ADDRESS)         //  if ((lsp_flag != 0)
    VoC_bngt16_rd(cl_ltp101,REG2,CONST_15565_ADDR);     // (sub(*gain_pit, GP_CLIP) > 0))

    /*       gpc_flag = check_gp_clipping(tonSt, *gain_pit);  move16 (); */
    VoC_shr16_ri(REG2,3,DEFAULT);       // sum = shr(g_pitch, 3);
    VoC_lw16i_set_inc(REG3,STRUCT_TONSTABSTATE_GP_ADDR,1);
    VoC_loop_i_short(7,DEFAULT)     // for (i = 0; i < N_FRAME; i++)
    VoC_startloop0
    VoC_add16inc_rp(REG2,REG2,REG3,DEFAULT);    //     sum = add(sum, st->gp[i]);
    VoC_endloop0

    VoC_bngt16_rd(cl_ltp101,REG2,CONST_15565_ADDR)              // if (sub(sum, GP_CLIP) > 0)
    VoC_lw16i_short(REG7,1,DEFAULT);// return 1;
cl_ltp101:
    /* REG7 for gpc_flag */
    /* special for the MR475, MR515 mode; limit the gain to 0.85 to */
    /* cope with bit errors in the decoder in a better way.         */
    VoC_lw16i(REG6,15565);  // reg6= GP_CLIP;
    VoC_bez16_r(cl_ltp202,REG7);                    // if (gpc_flag != 0)
    VoC_movreg16(RL7_LO,REG6,DEFAULT);      // *gain_pit = GP_CLIP;
    VoC_movreg16(RL7_HI,REG6,DEFAULT);      // *gp_limit = GP_CLIP;
cl_ltp202:
    VoC_sw16_d(RL7_LO,COD_AMR_GAIN_PIT_ADDRESS);
//cl_ltp1202:
    VoC_sw16_d(RL7_HI,COD_AMR_GP_LIMIT_ADDRESS);
//  VoC_jal(CII_q_gain_pitch_122);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_set_inc(REG0, STATIC_CONST_QUA_GAIN_PITCH_ADDR,1);
    VoC_lw16i_set_inc(REG2, 0, 1);
    VoC_lw16i(REG3, 0x7fff);
    VoC_loop_i_short(16, DEFAULT);   // for (i = 1; i < NB_QUA_PITCH; i++)   #define NB_QUA_PITCH  16
    VoC_lw16inc_p(REG1, REG0, IN_PARALLEL);
    VoC_startloop0
    VoC_bgt16_rd(q_gain_pitch_110, REG1, COD_AMR_GP_LIMIT_ADDRESS)
    VoC_sub16_dr(REG1, COD_AMR_GAIN_PIT_ADDRESS, REG1);
    VoC_bgtz16_r(q_gain_pitch_111, REG1)
    VoC_sub16_dr(REG1, CONST_0_ADDR, REG1);
q_gain_pitch_111:
    //REG1:  err
    VoC_bngt16_rr(q_gain_pitch_110, REG3, REG1)
    VoC_movreg16(REG3, REG1, DEFAULT);
    VoC_movreg16(REG4, REG2, IN_PARALLEL);
q_gain_pitch_110:
    VoC_inc_p(REG2, DEFAULT);
    VoC_lw16inc_p(REG1, REG0, IN_PARALLEL);
    VoC_endloop0

    //REG4  index
    VoC_lw16i_set_inc(REG0, STATIC_CONST_QUA_GAIN_PITCH_ADDR,1);
    VoC_add16_rr(REG1, REG0, REG4, DEFAULT);
    VoC_movreg16(REG3,REG4,  DEFAULT);
    //REG3   ii
    VoC_lw16inc_p(REG5, REG1, DEFAULT);

    VoC_and16_ri(REG5,0xFFFC);  // *gain = qua_gain_pitch[index] & 0xFFFC;
    exit_on_warnings = OFF;
    VoC_lw16_sd(REG7,5,DEFAULT);      //COD_AMR_ANAP_ADDR
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);
    VoC_sw16_p(REG4,REG7,DEFAULT);
    VoC_sw16_sd(REG7,5,DEFAULT);        //COD_AMR_ANAP_ADDR
    VoC_sw16_d(REG5, COD_AMR_GAIN_PIT_ADDRESS);
    exit_on_warnings = ON;

    /* update target vector und evaluate LTP residual */
    VoC_lw16_d(REG3,COD_AMR_GAIN_PIT_ADDRESS);
    VoC_lw16i_set_inc(REG0,COD_AMR_Y1_ADDRESS,2 );                  /* REG0 for y1[] first address */
    VoC_lw16i_set_inc(REG1,COD_AMR_XN_ADDRESS,2 );                  /* REG1 for xn[] first address */
    VoC_lw16i_set_inc(REG2,COD_AMR_XN2_ADDRESS,2 );             /* REG2 for xn2[] first address */

    VoC_loop_i(1,2)

    VoC_multf32inc_rp(REG45,REG3,REG0,DEFAULT);       // L_temp = L_mult(y1[i], *gain_pit);
    VoC_multf32inc_rp(REG67,REG3,REG0,IN_PARALLEL);

    VoC_loop_i_short(20,DEFAULT)                        /*for (i = 0; i < L_SUBFR; i++) */
    VoC_startloop0

    VoC_shr32_ri(REG45,-1,DEFAULT);                 // L_temp = L_shl(L_temp, 1);
    VoC_shr32_ri(REG67,-1,IN_PARALLEL);
    VoC_sub16inc_pr(REG4,REG1,REG5,DEFAULT);        // xn2[i] = sub(xn[i], extract_h(L_temp));
    VoC_sub16inc_pr(REG5,REG1,REG7,IN_PARALLEL);

    VoC_multf32inc_rp(REG45,REG3,REG0,DEFAULT);       // L_temp = L_mult(y1[i], *gain_pit);
    VoC_multf32inc_rp(REG67,REG3,REG0,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(REG45,REG2,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_lw16_d(REG0,COD_AMR_EXC_ADDR_ADDRESS);      /* REG0 for &exc[i_subfr] address */
    VoC_lw16i(REG1,COD_AMR_RES_ADDRESS);
    VoC_movreg16(REG2,REG1,DEFAULT);    /* REG1 and REG2 for res2[] first address */
    VoC_endloop1
//  end of CII_cl_ltp
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    /* update LTP lag history
    if ((subfrNr == 0) && (st->ol_gain_flg[0] > 0))
    {
       st->old_lags[1] = T0;     move16 ();
    }
    */
    VoC_lw16_sd(REG6,4,DEFAULT);
    VoC_lw16_d(REG7,COD_AMR_T0_ADDRESS);
    VoC_bnez16_r(cod_amr116,REG6)
    VoC_bngtz16_d(cod_amr116,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR+1);
cod_amr116:

    /*  if ((sub(subfrNr, 3) == 0) && (st->ol_gain_flg[1] > 0))
          {
             st->old_lags[0] = T0;     move16 ();
          }
    */
    VoC_bne16_rd(cod_amr117,REG6,CONST_3_ADDR)
    VoC_bngtz16_d(cod_amr117,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR+1)
    VoC_sw16_d(REG7,STRUCT_COD_AMRSTATE_OLD_LAGS_ADDR);
cod_amr117:
    /*-----------------------------------------------------------------*
     * - Inovative codebook search (find index and gain)               *
     *-----------------------------------------------------------------
    cbsearch(xn2, st->h1, T0, st->sharp, gain_pit, res2,
             code, y2, &ana, *usedMode, subfrNr);*/
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// begin of function    CII_cbsearch);

    VoC_lw16i_set_inc(REG0, COD_AMR_XN2_ADDRESS,1);
    VoC_sw16_d(REG6,COD_AMR_SUBFRNR_ADDRESS);
    VoC_lw16i_set_inc(REG1, STRUCT_COD_AMRSTATE_H1_ADDR,1);
    VoC_lw16d_set_inc(REG2, STRUCT_COD_AMRSTATE_SHARP_ADDR,1);
    VoC_lw16d_set_inc(REG3, COD_AMR_GAIN_PIT_ADDRESS,1);
    VoC_lw16i(REG4, COD_AMR_RES_ADDRESS);
    VoC_lw16i(REG5, COD_AMR_CODE_ADDRESS);

    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_push16(REG3,DEFAULT);
    VoC_push16(REG4,DEFAULT);
    VoC_push16(REG5,DEFAULT);

    VoC_lw16i(REG0, COD_AMR_Y2_ADDRESS);
    VoC_lw16_d(REG1, COD_AMR_SUBFRNR_ADDRESS);

    VoC_lw16_sd(REG2,11,DEFAULT); //COD_AMR_ANAP_ADDR
    VoC_push16(REG0,DEFAULT);
    VoC_push16(REG1,DEFAULT);
    VoC_push16(REG2,DEFAULT);
    VoC_lw16_sd(REG0, 7, DEFAULT);          // st->h1
    VoC_lw16_sd(REG3,5, DEFAULT);         // gain_pit
    VoC_jal(CII_cbseach_subr1);
//   VoC_counter_stats(voc_counter);
    VoC_jal(CII_code_10i40_35bits);
//   VoC_counter_stats(voc_counter);

    VoC_lw16_sd(REG0, 3, DEFAULT);          // code
    VoC_lw16_sd(REG3,5, DEFAULT);         // st->sharp
    VoC_jal(CII_cbseach_subr1);
    VoC_jal(CII_gainQuant_M122);
//stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap
    // if want to save anap, do here  (COD_AMR_ANAP_ADDR)

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0
    VoC_pop16(REG0, DEFAULT);
    VoC_endloop0

    VoC_lw16_d(REG6,COD_AMR_GAIN_PIT_ADDRESS);

    VoC_sw16_sd(REG5,5,DEFAULT);

// end of function  CII_cbsearch);
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


    /* update gain history */
//      update_gp_clipping(st->tonStabSt, gain_pit);
//   Copy(&st->gp[1], &st->gp[0], N_FRAME-1);
//   st->gp[N_FRAME-1] = shr(g_pitch, 3);

    VoC_shr16_ri(REG6,3,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,STRUCT_TONSTABSTATE_GP_ADDR+1,1);
    VoC_lw16i_set_inc(REG0,STRUCT_TONSTABSTATE_GP_ADDR,1);

    VoC_loop_i_short(6,DEFAULT)
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
    VoC_sw16_p(REG6,REG0,DEFAULT);

//EFR_DTX_PART1:
    /* Subframe Post Porcessing
         subframePostProc(st->speech, *usedMode, i_subfr, gain_pit,
                          gain_code, Aq, synth, xn, code, y1, y2, st->mem_syn,
                          st->mem_err, st->mem_w0, st->exc, &st->sharp);*/
    VoC_lw16i_short(REG0,0,DEFAULT);    // 0 for not sf
    VoC_lw16i(RL7_LO,STRUCT_COD_AMRSTATE_MEM_W0_ADDR);
    VoC_lw16i(RL7_HI,STRUCT_COD_AMRSTATE_MEM_SYN_ADDR);

    VoC_jal(CII_subframePostProc);

    VoC_lw16_sd(REG7,0,DEFAULT);    // i_subfr
    VoC_lw16i_short(REG0,40,IN_PARALLEL);
    VoC_add16_rr(REG7,REG7,REG0,DEFAULT);
    VoC_lw16_sd(REG5,2,IN_PARALLEL);    // A_Q
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_sd(REG6,1,IN_PARALLEL);    // evensb
    VoC_sub16_rr(REG6,REG1,REG6,DEFAULT);// evenSubfr = sub(1, evenSubfr);
    VoC_sw16_sd(REG7,0,DEFAULT);    // i_subfr
    VoC_lw16i_short(REG2,11,IN_PARALLEL);
    VoC_sw16_sd(REG6,1,DEFAULT);    // evensb
    VoC_add16_rr(REG5,REG5,REG2,IN_PARALLEL);
    VoC_lw16_sd(REG4,3,DEFAULT);    // A
    VoC_sw16_sd(REG5,2,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG2,IN_PARALLEL);
    VoC_lw16_sd(REG3,4,DEFAULT);    // subNr     // subfrNr = add(subfrNr, 1);
    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_sw16_sd(REG4,3,DEFAULT);
    VoC_sw16_sd(REG3,4,DEFAULT);

    VoC_jump(cod_amr112);
cod_amr111:
    VoC_loop_i_short(5,DEFAULT)
    VoC_startloop0
    VoC_pop16(REG7,DEFAULT);
    VoC_endloop0

//   Copy(&st->old_exc[L_FRAME], &st->old_exc[0], PIT_MAX + L_INTERPOL);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_EXC_ADDR+160,2);
    VoC_lw16i_set_inc(REG1,STRUCT_COD_AMRSTATE_OLD_EXC_ADDR,2);
    exit_on_warnings = OFF;
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_loop_i(1,77)
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_sw32inc_p(RL7,REG1,DEFAULT);
    VoC_endloop1
    exit_on_warnings = ON;

//the_end:

    VoC_jal(CII_PRM2_BITS_POST_PROCESS);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}



//INPUT:  RL7_LO :  nb_track   RL7_HI  :   40/step

void CII_set_sign12k2_122 (void)
{

    VoC_push16(RA,DEFAULT);
//stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA, RA

    /* calculate energy for normalization of cn[] and dn[] */


    VoC_lw16_sd(REG0, 6, DEFAULT);                    // incr0 = 2           /* REG2 for cn[] first address */

    VoC_loop_i(1,2)
    VoC_lw16i_short(ACC0_HI, 0,DEFAULT);
    VoC_lw16i(ACC0_LO ,256);                      /* ACC0 for s  s = 256 */
    VoC_loop_i_short(20,DEFAULT);                       /*for (i = 0; i < L_CODE; i++)*/

    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);               /*s = L_mac (s, cn[i], cn[i]);*/
    VoC_endloop0
    VoC_movreg16(REG4, REG1, DEFAULT);
    VoC_movreg32(REG01,ACC0,DEFAULT);
    VoC_jal(CII_Inv_sqrt);

    VoC_shr32_ri(REG01,-5,DEFAULT);             /* L_shl (s, 5) */
    VoC_lw16i_set_inc(REG0,C1035PF_DN_ADDR,2);

    VoC_endloop1
    VoC_movreg16(REG5, REG1, DEFAULT);
    VoC_movreg16(REG2, REG0, IN_PARALLEL);
    //REG2    C1035PF_DN_ADDR

    VoC_lw16i(ACC1_LO,32767);
    VoC_lw16i(ACC1_HI,-32767);
    VoC_lw16i_set_inc(REG0,C1035PF_SIGN_ADDR-1,1);                   /* REG0 for sign[-1]*/


    VoC_lw16_sd(REG3, 6, DEFAULT);               // RES2     incr3 =1    /* REG3 for cn[] first address */

    VoC_lw16i(REG1,SET_SIGN_EN_ADDR);              /* REG1 for en[] first address */

    //for 0 and -32767

    VoC_lw32_d(RL6,CONST_0x00008000_ADDR);

    VoC_lw16i_short(FORMAT32,-11,DEFAULT);
    VoC_loop_i_short(40,DEFAULT);                     /*    for (i = 0; i < L_CODE; i++)  */
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);
    VoC_startloop0
    VoC_mac32inc_rp(REG4,REG3,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);

    VoC_mac32_rp(REG5,REG2,DEFAULT);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_sw16_p(ACC1_LO,REG0,IN_PARALLEL);

    VoC_movreg16(REG7, ACC0_HI,DEFAULT);
    VoC_movreg32(ACC0, RL6,IN_PARALLEL);

    VoC_bnltz16_r(SET_SIGN101,REG7)

    VoC_sub16_rr(REG7,REG6,REG7,DEFAULT);                   /*cor = negate (cor);*/
    /* modify dn[] according to the fixed sign */
    VoC_sub16_rp(REG6,REG6,REG2,IN_PARALLEL);               /*val = negate (val);*/
    VoC_sw16_p(ACC1_HI,REG0,DEFAULT);           /*sign[i] = -32767; */
    VoC_sw16_p(REG6,REG2,DEFAULT);
SET_SIGN101:
    VoC_sw16inc_p(REG7,REG1,DEFAULT);           /* en[i] = cor;*/
    VoC_inc_p(REG2,DEFAULT);
    VoC_endloop0


    VoC_lw16i_short(FORMAT32,-1,DEFAULT);


//RL7_LO :  nb_track   RL7_HI  :   40/step
    VoC_movreg16(REG6, RL7_LO, DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);                     /* REG0 for i */

    VoC_lw16i_set_inc(REG1,C1035PF_POS_MAX_ADDR,1);               /* REG1:&pas_max[] */


    VoC_movreg16(INC2,REG6,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i_short(REG4,-1,DEFAULT);                                    /* REG4 for max_of_all*/
    //REG4   max_of_all

    VoC_loop_r(1,REG6)
    VoC_lw16i(REG2, SET_SIGN_EN_ADDR);
    VoC_add16_rr(REG2,REG2,REG0,DEFAULT);/* REG2 for &en[j]*/
    VoC_movreg16(REG6, RL7_HI, IN_PARALLEL);

    VoC_loop_r_short(REG6,DEFAULT);
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);                             /* REG5 for max max = -1; */
    //REG5   max
    VoC_startloop0
    VoC_lw16_p(REG6,REG2,DEFAULT);          /* REG6 for cor   cor = en[j];  */
    VoC_bngt16_rr(SET_SIGN107,REG6,REG5)                /* if (val <= 0)*/
    /* max = cor; */
    VoC_lw16i(REG3, SET_SIGN_EN_ADDR);
    VoC_movreg16(REG5,REG6,DEFAULT);
    // REG3 for j
    VoC_sub16_rr(REG3,REG2,REG3,IN_PARALLEL);   //pos=j+5
SET_SIGN107:
    VoC_inc_p(REG2, DEFAULT);

    VoC_endloop0


    VoC_bngt16_rr(SET_SIGN104,REG5,REG4)             /* if (val <= 0)*/
    VoC_movreg16(REG4,REG5,DEFAULT);            /*max_of_all = max;  */
    /* starting position for i0 */
    /* ipos[0] = i;  */
    VoC_movreg16(REG7,REG0,IN_PARALLEL);
SET_SIGN104:;

    /* store maximum correlation position */
    VoC_inc_p(REG0,DEFAULT);                    //i++
    VoC_sw16inc_p(REG3,REG1,IN_PARALLEL);               /*pos_max[i] = pos;  */
    VoC_endloop1

    /*----------------------------------------------------------------*
     *     Set starting position of each pulse.                       *
     *----------------------------------------------------------------*/
    VoC_lw16i_set_inc(REG1,C1035PF_IPOS_ADDR,1);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG4, RL7_LO, IN_PARALLEL);

    /* ipos[5] = pos;   */

    /* REG1 for ipos[1] */
    VoC_loop_r_short(REG4,DEFAULT)                          /* for (i = 1; i < NB_TRACK; i++)*/
    VoC_add16_rr(REG3, REG1, REG4, IN_PARALLEL);
    VoC_startloop0

    VoC_sw16inc_p(REG7,REG1,DEFAULT);           /*ipos[i] = pos;   */
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_1_ADDR);             /* pos = add (pos, 1);*/
    /* if (sub (pos, NB_TRACK) >= 0) */
    VoC_bgt16_rr(SET_SIGN106,REG4,REG7)
    VoC_lw16i_short(REG7,0,DEFAULT);                            /*pos = 0;                  */
SET_SIGN106:
    VoC_NOP();
    VoC_endloop0
    VoC_return;
}



/*************************************************************************
 *
 *  FUNCTION:  code_10i40_35bits()
 *
 *  PURPOSE:  Searches a 35 bit algebraic codebook containing 10 pulses
 *            in a frame of 40 samples.
 *
 *  DESCRIPTION:
 *    The code contains 10 nonzero pulses: i0...i9.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    The 40 positions in a subframe are divided into 5 tracks of
 *    interleaved positions. Each track contains two pulses.
 *    The pulses can have the following possible positions:
 *
 *       i0, i5 :  0, 5, 10, 15, 20, 25, 30, 35.
 *       i1, i6 :  1, 6, 11, 16, 21, 26, 31, 36.
 *       i2, i7 :  2, 7, 12, 17, 22, 27, 32, 37.
 *       i3, i8 :  3, 8, 13, 18, 23, 28, 33, 38.
 *       i4, i9 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *    Each pair of pulses require 1 bit for their signs and 6 bits for their
 *    positions (3 bits + 3 bits). This results in a 35 bit codebook.
 *    The function determines the optimal pulse signs and positions, builds
 *    the codevector, and computes the filtered codevector.
 *
 *************************************************************************/

void CII_code_10i40_35bits (void)
{
    VoC_push16(RA,DEFAULT);
//stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA

    VoC_lw16i_short(ACC1_LO,2, DEFAULT);              //ACC1_LO  : sf
    VoC_lw16i_short(REG4,5, DEFAULT);          // REG4  :nb_track & step
    VoC_lw16i_short(REG5,8, DEFAULT);                    //REG5  : 40/step        // incr0 and  incr1 =1
    VoC_jal(CII_cor_h_x);                                         //    cor_h_x (h, x, dn, 2);

    //INPUT:  RL7_LO :  nb_track   RL7_HI  :   40/step
    VoC_lw16i_short(RL7_LO,5,DEFAULT);            //NB_TRACK   STEP
    VoC_lw16i_short(RL7_HI,8,IN_PARALLEL);
    VoC_jal(CII_set_sign12k2_122);                          //    set_sign12k2 (dn, cn, sign, pos_max, NB_TRACK, ipos, STEP);

    VoC_jal(CII_cor_h);                           //    cor_h (h, sign, rr);

    VoC_jal(CII_search_M122);

    //   build_code (codvec, sign, cod, h, y, indx);
    VoC_jal(CII_build_code_M122);

    VoC_lw16_sd(REG0,1,DEFAULT);      //  ANAP
    VoC_lw16i_short(REG1, 0, IN_PARALLEL);   //incr1=1



    VoC_loop_i_short(10, DEFAULT);
    VoC_lw16i_short(REG5,8, IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG7, REG0, DEFAULT);
    VoC_lw16i_short(REG6, 7, IN_PARALLEL);

    VoC_and16_rr(REG6, REG7,DEFAULT);
    VoC_add16_rr(REG6, REG2, REG6, DEFAULT);

    VoC_and16_rr(REG7, REG5,DEFAULT);

    VoC_lw16_p(REG6, REG6, DEFAULT);
    VoC_or16_rr(REG7, REG6, DEFAULT);
    VoC_bnlt16_rd(q_p_100, REG1, CONST_5_ADDR)
    VoC_movreg16( REG6,REG7, DEFAULT);
q_p_100:
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(REG6,REG0,DEFAULT);
    VoC_endloop0

    VoC_pop16(RA,DEFAULT);
    VoC_sw16_sd(REG0,0,DEFAULT);         //  REstore ANAP
    VoC_return;
}




void CII_build_code_M122 (void)
{
    //stack16:   RA,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,  RA,RA
    VoC_push16(RA,DEFAULT);
    VoC_lw16_sd(REG0, 5, DEFAULT);              // code
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw32z(ACC0,DEFAULT);

    VoC_loop_i_short(20,DEFAULT);
    VoC_lw16i_short(REG2,-1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_endloop0

    VoC_lw16i(ACC1_LO,cbsearch_dn_sign_ADDR);

    VoC_lw16_sd(REG3, 2, DEFAULT);              // anap    incr1 =1
    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw16_sd(ACC1_HI, 5, IN_PARALLEL);           // code
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw16i(RL6_LO, -8192);
    VoC_lw16i(RL6_HI, 8192);

    VoC_lw16i_set_inc(REG0,C1035PF_CODVEC_ADDR,1);
    VoC_lw16i_set_inc(REG1,build_code_sign_ADDR,1);

    VoC_loop_i(0,10)
    VoC_lw16_d(REG4,CONST_0x199a_ADDR);
    VoC_multf16_rp(REG6, REG4,REG0,DEFAULT );  // 6554 ,REG6   index
    VoC_lw16inc_p(REG4, REG0,DEFAULT);      //REG4    i
    VoC_multf32_rd(REG23,REG6,CONST_5_ADDR);
    VoC_sw16_p(RL6_HI, REG1,DEFAULT);
    VoC_shr32_ri(REG23, 1, DEFAULT);
    VoC_sub16_rr(REG5,REG4, REG2, DEFAULT);     //REG5   track
    VoC_movreg32(REG23, ACC1, IN_PARALLEL);
    VoC_add16_rr(REG2, REG2,REG4, DEFAULT);         //REG2    cbsearch_dn_sign_ADDR
    VoC_add16_rr(REG3, REG3,REG4, DEFAULT);     //REG3     COD_AMR_CODE_ADDRESS


    VoC_lw16_p(REG2, REG2, DEFAULT);        //REG2       j
    VoC_add16_pd(REG7, REG3, CONST_4096_ADDR);

    VoC_bgtz16_r(build_code_M122_100_end, REG2)
    VoC_sub16_rd(REG7, REG7, CONST_0x2000_ADDR);    // 2*4096
    VoC_sw16_p(RL6_LO, REG1,DEFAULT);
    VoC_add16_rd(REG6, REG6, CONST_8_ADDR);
build_code_M122_100_end:
    VoC_lw16_sd(REG2, 2, DEFAULT);              // anap
    VoC_inc_p(REG1,IN_PARALLEL);
    VoC_sw16_p(REG7, REG3,DEFAULT);
    VoC_add16_rr(REG2, REG2, REG5,IN_PARALLEL);     //REG2->  indx[track]
    VoC_add16_rd(REG3, REG2, CONST_5_ADDR);     //REG3 ->  indx[track + 5]
    VoC_lw16_p(REG7, REG2, DEFAULT);
    VoC_bltz16_r(build_code_M122_200_end, REG7);
    VoC_movreg16(REG5, REG6, DEFAULT);
    VoC_xor16_rr(REG5, REG7, DEFAULT);
    VoC_sub16_rr(REG4, REG7, REG6, IN_PARALLEL);    //REG4  (sub (indx[track], index)
    VoC_and16_ri(REG5, 8);                      //REG5     (index ^ indx[track]) & 8)
    VoC_bnez16_r(build_code_M122_300, REG5);
    VoC_bgtz16_r(build_code_M122_400, REG4);
build_code_M122_500:
    VoC_movreg16(REG2, REG3,DEFAULT);
    VoC_jump(build_code_M122_200_end);
build_code_M122_300:
//      VoC_movreg16(REG5, REG7, DEFAULT);
//      VoC_movreg16(REG4, REG6, IN_PARALLEL);
//      VoC_and32_rd(REG45,CONST_0x00070007_ADDR);
    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,7,DEFAULT);
    VoC_and32_rr(REG45,REG67,DEFAULT);

    VoC_sub16_rr(REG4, REG5, REG4, DEFAULT);         //REG4    (sub ((indx[track] & 7), (index & 7))
    VoC_bgtz16_r(build_code_M122_500, REG4);
build_code_M122_400:
    VoC_sw16_p(REG7, REG3,DEFAULT);
build_code_M122_200_end:
    VoC_sw16_p(REG6, REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16_sd(REG7, 9, DEFAULT);           //st->h1
    VoC_lw16_sd(REG0, 4, DEFAULT);         // Y2
    VoC_lw16i_short(REG6,10,IN_PARALLEL);
    /**********************************************
      Function CII_build_code_com2
      input: REG6->NB_PULSE
         REG7->h[]
         REG0(incr=1)->y[]
      Note: This function can used in all mode
    **********************************************/
    VoC_jal(CII_build_code_com2);
    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_set_inc(REG2,STATIC_CONST_gray_ADDR, 1);
    VoC_return;
}







void CII_search_M122(void)  //CII_search_M102
{




//  search_count++;
    /*my pointers*/
    VoC_push16(RA,DEFAULT);


    VoC_lw16i_set_inc(REG1,SEARCH_CODE_CP_RR_ADDR,1);

    VoC_lw16i(REG4,C1035PF_RR_SIDE_ADDR );
    VoC_lw16i(REG5,C1035PF_RR_DIAG_ADDR  );

    VoC_lw16i_set_inc(REG2,C1035PF_DN_ADDR,1);          //&rr[0] and &dn[0]

    VoC_sw32_d(REG45,C1035PF_RR_SIDE_ADDR_ADDR);
    VoC_movreg16(REG0, REG5, DEFAULT);


    VoC_lw16i_set_inc(REG3,SEARCH_10I40_RRV_ADDR,1);
    VoC_sw32_d(REG01,C1035PF_RR_DIAG_ADDR_2_ADDR);

    /*end of my pointers*/

    /* fix i0 on maximum of correlation position */

    VoC_lw16i(REG0,C1035PF_POS_MAX_ADDR);
    VoC_add16_rd(REG0,REG0,C1035PF_IPOS_D_ADDR);

    VoC_sw32_d(REG23,SEARCH_CODE_DN_ADDR_ADDR);             //&cp_rr[0] and &rrv[0]


    VoC_lw16_p(REG2,REG0,DEFAULT);          // REG2 value of pos_max[ipos[0]]

//  SEARCH_CODE_ALP0_TEMP_ADDR  SEARCH_CODE_PS0_TEMP_ADDR
    /*ps0_temp:dn[i0]; alp0:L_mult (rr[i0][i0], _1_16)+0x8000;*/
    VoC_add16_rd(REG0,REG2,SEARCH_CODE_DN_ADDR_ADDR);   //&dn[i0]

    VoC_sw16_d(REG2,SEARCH_10I40_I0_ADDR);      //i0 = pos_max[ipos[0]];

    VoC_add16_rd(REG1,REG2,C1035PF_RR_DIAG_ADDR_ADDR);  //&rr[i0][i0]

    VoC_lw16_p(REG6,REG0,DEFAULT);                                          //dn[i0]

    //VoC_multf32_pd(ACC0,REG1,CONST_1_16_ADDR);   gugogi
    VoC_lw16_d(REG3,CONST_1_16_ADDR);
    VoC_multf32_rp(ACC0,REG3,REG1,DEFAULT);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR,8);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    VoC_sw16_d(REG6,SEARCH_CODE_PS0_TEMP_ADDR);
    VoC_sw32_d(ACC0,SEARCH_CODE_ALP0_TEMP_ADDR);        //  rr[i0][i0] *  _1_16  + 0x00008000

    /*copy rr[i0] to cp_rr*/


    VoC_jal(CII_SEARCH_COPY);

    /*------------------------------------------------------------------*
     * i1 loop:                                                         *
     *------------------------------------------------------------------*/

    VoC_lw16i_short(REG4,-1,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,1); //REG1 addr of codvec[0]

    VoC_loop_i_short(10,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_startloop0
    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_endloop0

    VoC_lw16i_short(REG2,5,DEFAULT);

    VoC_sw32_d(REG45,SEARCH_10I40_PSK_ADDR);  //alpk = 1   //psk =-1


SEARCH_10I40_LOOP:

    VoC_lw16i(REG3,C1035PF_POS_MAX_ADDR); //REG2 addr of pos_max[0]
    VoC_add16_rd(REG3,REG3,C1035PF_IPOS_D_ADDR+1);//REG2 addr of pos_max[ipos[1]]

    /*copy rr[i1] to cp_rr*/

    VoC_push16(REG2,DEFAULT);

    VoC_lw16_p(REG2,REG3,DEFAULT);                      // REG2 value of pos_max[ipos[1]]
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+1,8);
    VoC_sw16_d(REG2,SEARCH_10I40_I1_ADDR);              //i1 = pos_max[ipos[1]];
    VoC_jal(CII_SEARCH_COPY);


    /*compute rrv*/
    VoC_lw16i_set_inc(REG3,CONST_D_1_4_ADDR,0);     //reg3:&double_1_4

    VoC_lw16i_short(REG4,1,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG7,38,IN_PARALLEL);
    VoC_lw16i(REG5,_1_8);                   //reg5:_1_8
    VoC_lw16_d(REG0,C1035PF_IPOS_D_ADDR+3);         //reg6:ipos[3]


    VoC_jal(CII_COMPUTE_RRV_M122);

    /*ps0 = add (dn[i0], dn[i1]);*/
    VoC_lw16d_set_inc(REG1,SEARCH_10I40_I1_ADDR,1);     //reg1:i1

    VoC_add16_rd(REG0,REG1,SEARCH_CODE_DN_ADDR_ADDR);   //REG0:&dn[i1]
    VoC_mult16_rd(REG1,REG1,CONST_8_ADDR);
    VoC_add16_pd(REG7,REG0,SEARCH_CODE_PS0_TEMP_ADDR);
    VoC_add16_rd(REG1,REG1,SEARCH_CODE_CP_RR_ADDR_ADDR);    //reg1:&cp_rr[i0][i1]
    VoC_lw32_d(ACC0,SEARCH_CODE_ALP0_TEMP_ADDR);
    VoC_mac32inc_rp(REG5,REG1,DEFAULT);         // reg5=_1_8
    VoC_mac32inc_pd(REG1,CONST_1_16_ADDR);
    VoC_sw16_d(REG7,SEARCH_10I40_PS0_ADDR);

    /*compute alp1*/
    VoC_lw16i_set_inc(REG3,CONST_D_1_8_ADDR,0);     //reg3:&double_1_8

    VoC_lw16i_short(REG2,1,DEFAULT);            //reg2:loop num
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000

    VoC_lw16i_short(REG5,42,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+2);           //reg6:i2
    VoC_lw16i(REG7,_1_16);                  //reg7:_1_16

    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
    VoC_lw16i(REG0,0x4000);
    VoC_lw16i(REG1,_1_8);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+2);        //rl7:  ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    VoC_sw32_d(RL7,SEARCH_10I40_I2_ADDR);

    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000


    /*copy rr[i2],rr[i3] to rr_cp*/
    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+2,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);

    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+3,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    /*compute rrv*/
    VoC_lw16i_set_inc(REG3,CONST_D_1_4_ADDR,0);     //reg3:&double_1_4
    VoC_lw16i_short(REG4,2,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG7,36,IN_PARALLEL);

    VoC_lw16i(REG5,_1_8);                   //reg5:_1_8
    VoC_lw16_d(REG0,C1035PF_IPOS_D_ADDR+5);         //reg6:ipos[5]

    VoC_jal(CII_COMPUTE_RRV_M122);


    /*compute alp1*/

    VoC_lw16i_set_inc(REG3,CONST_D_1_16_ADDR,0);        //reg3:&double_1_16
    VoC_lw16i_short(REG2,2,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG5,44,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+4);           //reg6:i4
    VoC_lw16i(REG7,_1_32);                  //reg7:_1_32
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
    VoC_lw16i(REG0,_1_4);
    VoC_lw16i(REG1,_1_16);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+4);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);


    VoC_sw32_d(RL7,SEARCH_10I40_I4_ADDR);


    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000

    /*copy rr[i4],rr[i5] to rr_cp*/

    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+4,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+5,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    /*compute rrv*/
    VoC_lw16i_set_inc(REG3,CONST_D_1_8_ADDR,0);     //reg3:&double_1_8
    VoC_lw16i_short(REG4,3,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG7,34,IN_PARALLEL);
    VoC_lw16i(REG5,_1_16);                  //reg5:_1_16
    VoC_lw16_d(REG0,C1035PF_IPOS_D_ADDR+7);         //reg6:ipos[7]

    VoC_jal(CII_COMPUTE_RRV_M122);

    /*compute alp1*/

    VoC_lw16i_set_inc(REG3,CONST_D_1_32_ADDR,0);        //reg3:&double_1_32
    VoC_lw16i_short(REG2,3,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG5,46,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+6);           //reg6:i6
    VoC_lw16i(REG7,_1_64);                  //reg7:_1_64
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
    VoC_lw16i(REG0,_1_4);
    VoC_lw16i(REG1,_1_32);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+6);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    VoC_sw32_d(RL7,SEARCH_10I40_I6_ADDR);
    /* now finished searching a set of 8 pulses */

    VoC_sw16_d(RL6_LO,SEARCH_10I40_PS0_ADDR);
    VoC_push32(ACC0,DEFAULT);               //alp0+0x8000

    /*copy rr[i6],rr[i7] to rr_cp*/
    VoC_movreg16(REG2, RL7_LO,IN_PARALLEL);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+6,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);

    VoC_movreg16(REG2, RL7_HI,DEFAULT);
    VoC_lw16i_set_inc(REG3,SEARCH_CODE_CP_RR_ADDR+7,8); //reg2:dest addr
    VoC_jal(CII_SEARCH_COPY);
    /*compute rrv*/
    VoC_lw16i_set_inc(REG3,CONST_D_1_8_ADDR,0);     //reg3:&double_1_8
    VoC_lw16i_short(REG4,4,DEFAULT);            //REG4:loop num
    VoC_lw16i_short(REG7,32,IN_PARALLEL);
    VoC_lw16i(REG5,_1_16);                  //reg5:_1_16
    VoC_lw16_d(REG0,C1035PF_IPOS_D_ADDR+9);         //reg6:ipos[9]


    VoC_jal(CII_COMPUTE_RRV_M122);

    /*compute alp1*/

    VoC_lw16i_set_inc(REG3,CONST_D_1_64_ADDR,0);        //reg3:&double_1_64
    VoC_lw16i_short(REG2,4,DEFAULT);            //reg2:loop num
    VoC_lw16i_short(REG5,48,IN_PARALLEL);

    VoC_lw16_d(REG6,C1035PF_IPOS_ADDR+8);           //reg6:i8
    VoC_lw16i(REG7,_1_128);                 //reg7:_1_128
    VoC_jal(CII_COMPUTE_ALP1_M122);

    /* Default value */
    VoC_lw16i(REG0,_1_8);
    VoC_lw16i(REG1,_1_64);
    VoC_lw32_d(RL7,C1035PF_IPOS_ADDR+8);            //rl7:      ia and ib
    VoC_jal(CII_SEARCH_LOOP_M122);

    /*----------------------------------------------------------------*
    * memorise codevector if this one is better than the last one.   *
    *----------------------------------------------------------------*/

    VoC_multf32_rd(ACC0,REG6,SEARCH_10I40_ALPK_ADDR);
    VoC_msu32_rd(REG7,SEARCH_10I40_PSK_ADDR);

    VoC_sw32_d(RL7,SEARCH_10I40_I8_ADDR);

    VoC_bngtz32_r(SEARCH_CODE_NOMEMORISE,ACC0)
    VoC_sw32_d(REG67,SEARCH_10I40_PSK_ADDR);    //PSK  ALPK
    VoC_lw16i_set_inc(REG0,SEARCH_10I40_I0_ADDR,2);
    VoC_lw16i_set_inc(REG1,C1035PF_CODVEC_ADDR,2);
    VoC_loop_i_short(5,DEFAULT)
    VoC_lw32inc_p(ACC0,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0
SEARCH_CODE_NOMEMORISE:
    /*----------------------------------------------------------------*
     * Cyclic permutation of i1,i2,i3,i4,i5,i6,i7,(i8 and i9).          *
     *----------------------------------------------------------------*/
    VoC_lw16i_set_inc(REG1,C1035PF_IPOS_ADDR+1,1);
    VoC_lw16i_set_inc(REG0,C1035PF_IPOS_ADDR+2,1);

    VoC_lw16_p(REG6,REG1,DEFAULT);  //ipos[1]

    VoC_loop_i_short(8,DEFAULT);
    VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG7,REG0,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_sw16_p(REG6,REG1,DEFAULT);
    VoC_pop16(REG2,DEFAULT);
    VoC_sub16_rd(REG2, REG2, CONST_1_ADDR);
    VoC_bngt16_rd(SEARCH_10I40_LOOP1,REG2,CONST_1_ADDR);
    VoC_jump(SEARCH_10I40_LOOP);
SEARCH_10I40_LOOP1:

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_return;
}


/*need reg3,reg4,reg5,reg6,rl6_hi,*/
void CII_COMPUTE_RRV_M122(void) //COMPUTE_RRV_M102
{

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_mult16_rd(REG1,REG0,CONST_8_ADDR);
    VoC_add16_rd(REG2,REG0,SEARCH_CODE_RRV_ADDR_ADDR);       //reg2:&rrv[i3]
    VoC_add32_rd(REG01,REG01,C1035PF_RR_DIAG_ADDR_2_ADDR);


    VoC_lw16i_short(INC0,5,DEFAULT);
    //reg0:&rr[i3][i3],inc 205;reg1:&cp_rr[i0][i3]

    //at least 4 instructions in the loop
    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);

    VoC_mac32inc_rp(REG5,REG0,DEFAULT);
    VoC_lw16i_short(INC2,5,IN_PARALLEL);
    VoC_loop_i(1,8)

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_bimac32inc_pp(REG3,REG1);
    VoC_endloop0

    VoC_add16_rr(REG1,REG1,REG7,DEFAULT);

    VoC_lw32_d(ACC0,CONST_0x00008000_ADDR);
    exit_on_warnings=OFF;
    VoC_mac32inc_rp(REG5,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_HI,REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop1
    VoC_return
}



/*need reg2,reg3,reg5,reg6,reg7,RL6(stack32)*/
/*after popping alp1 a more pop32 is needed*/
void CII_COMPUTE_ALP1_M122(void)    //COMPUTE_ALP1_M102
{

    VoC_pop32(RL6,DEFAULT);                //rl6:alp0+0x8000
    VoC_lw16i_set_inc(REG0, C1035PF_RR_DIAG_ADDR+35, -5);
    VoC_add16_rr(REG0,REG6,REG0,DEFAULT);

    VoC_mult16_rd(REG6,REG6,CONST_8_ADDR);
    VoC_lw16i_set_inc(REG1, SEARCH_CODE_CP_RR_ADDR+35*8, 2);

    VoC_add16_rr(REG1,REG6,REG1,DEFAULT);

    VoC_loop_i(1,8)
    VoC_loop_r_short(REG2,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG7,REG0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG3,REG1);
    VoC_endloop0
    VoC_push32(RL7,DEFAULT);

    VoC_sub16_rr(REG1,REG1,REG5,DEFAULT);
    VoC_add32_rr(RL7,ACC0,RL6,IN_PARALLEL);
    VoC_endloop1
    VoC_NOP();
    VoC_push32(RL7,DEFAULT);
    VoC_return

}




//output:
//r4:ps;rl_7:ia and ib;acc0:new alp0
void CII_SEARCH_LOOP_M122(void)
{


    VoC_movreg32(REG23, RL7,DEFAULT);
    VoC_movreg16(REG0,RL7_HI,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_d(REG01,SEARCH_CODE_RRV_COE_ADDR);
    exit_on_warnings=ON;

    VoC_movreg16(REG1,RL7_HI,DEFAULT);
    VoC_add32_rd(REG01,REG01,SEARCH_CODE_DN_ADDR_ADDR);


    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);        //reg67:    sq  and alp

    VoC_sw32_d(REG01,SEARCH_CODE_DN_RRV_ADDR_ADDR);      //reg01:&dn[i3] and &rrv[i3]

    VoC_loop_i(1,8)

    VoC_lw16i_set_inc(REG3, SEARCH_CODE_CP_RR_ADDR+7,8);

    VoC_add16_rd(REG5,REG2,SEARCH_CODE_DN_ADDR_ADDR);            // REG3  :  &dn[i2]

    VoC_sw16_d(REG2,SEARCH_10I40_I2_TEMP_ADDR);

//  copy the  rr[i2][]  to  cp_rr[][7] , and use  cp_rr[][7] to address  rr[i2][i3]


/////////////////////////////////////////////////////
// IN:
//     REG3:dest addr
//     REG2:i2
//     REG4:ipos[3]
//     INCR1 =-1
//     INCR3 =1
// OUT:
//     NONE
// REG USED:
//     REG0,REG1,REG2,REG3,REG4,REG5
// /////////////////////////////////////////////////
    // INCR0=5

    VoC_lw16i_short(INC0,5,DEFAULT);
    // REG0 for i2*i2
    VoC_mult16_rr(REG0, REG2, REG2,DEFAULT);
    // REG1 for i2
    VoC_movreg16(REG1, REG2,DEFAULT);

    // REG0 for i2*(i2-1)
    VoC_sub16_rr(REG0, REG0, REG2,DEFAULT);


    // REG0 for i2*(i2-1)/2
    VoC_shr16_ri(REG0,1,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);

    // REG0 for &rr_side[i2-1][0](rr[i2][0])
    // REG1 for &rr_diag[i2]
    VoC_add32_rd(REG01, REG01, C1035PF_RR_SIDE_ADDR_ADDR);

    // REG4 for &rr_side[i2-1][0]+i2
    VoC_add16_rr(REG4,REG0,REG2,DEFAULT);


    // REG0 for &rr_side[i2-1][ipos(3)]                 (rr[i2][ipos(3)]

    VoC_add16_rd(REG0,REG0,SEARCH_CODE_DN_RRV_ADDR_ADDR+1);   //  SEARCH_CODE_DN_RRV_ADDR_ADDR+1  is  ipos(3)

    VoC_lw16i(REG5,SEARCH_10I40_RRV_ADDR);

    VoC_sub16_rr(REG0,REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG5,5,IN_PARALLEL);

    // REG4 for &rr_side[i2-1][0]+i2+5
    // prepare for comparison because the REG0 will be pre-incremented 5
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    // REG5 for  &rr_diag[i2]
    VoC_movreg16(REG5,REG1,IN_PARALLEL);

    // REG2 loop count
    VoC_lw16i_short(REG1,8,DEFAULT);
    // REG4 for rr_side[i2-1][ipos(3)]
    // REG0 for &rr_side[i2-1][ipos(3)]+5
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);

    exit_on_warnings=OFF;
    // save &rr_side[i2-1][0]+i2 to RR_SIDE_DIAG_TEMP_ADDR
    VoC_sw16_d(REG4,RR_SIDE_DIAG_TEMP_ADDR);
    exit_on_warnings=ON;

    // jump if i2<=i3 (&rr_side[i2-1][0]+i2<=&rr_side[i2-1][ipos(3)])

    VoC_bnlt16_rd(SEARCH_COPY_label1, REG0, RR_SIDE_DIAG_TEMP_ADDR)

    // loop 8 time, maybe break if i3>=i2

    VoC_loop_i_short(8,DEFAULT);
    VoC_startloop0;
    // jump if i2<=i3 (&rr_side[i2-1][0]+i2+5<=&rr_side[i2-1][ipos(3)]+5)

    VoC_bnlt16_rd(SEARCH_COPY_label1, REG0, RR_SIDE_DIAG_TEMP_ADDR)

    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    // loop count - 1
    VoC_inc_p(REG1,DEFAULT);
    // REG0 is incremented 5(STEP) each time
    VoC_lw16inc_p(REG4, REG0,IN_PARALLEL);
    VoC_endloop0
SEARCH_COPY_label1:
    // jump if i2!=i3 (&rr_side[i2-1][0]+i2+5<=&rr_side[i2-1][ipos(3)]+5)
    VoC_bne16_rd( SEARCH_COPY_label2,REG0,RR_SIDE_DIAG_TEMP_ADDR);
    // REG4 fo rr_diag[i2]
    VoC_lw16_p(REG4, REG5,DEFAULT);
    // loop count -1
    VoC_inc_p(REG1,DEFAULT);
    VoC_sw16inc_p(REG4, REG3,DEFAULT);


SEARCH_COPY_label2:
    // REG2 is incremented by 25

    VoC_lw16i_short(INC2,25,DEFAULT);
    // REG4 for i3-i2
    VoC_sub16_rd(REG4,REG0,RR_SIDE_DIAG_TEMP_ADDR);
    // REG4 for i3
    VoC_add16_rr(REG4,REG4,REG2,DEFAULT);


    // REG0 for i3*i3
    VoC_mult16_rr(REG0,REG4,REG4,DEFAULT);
    VoC_lw16i_short(REG5,5,DEFAULT);
    // REG0 for i3*(i3-1)

    VoC_sub16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_lw16i_short(INC1,5,IN_PARALLEL);
    VoC_bez16_r(SEARCH_COPY_label_end, REG1)
    // REG0 for (i3*i3-1)/2
    VoC_shr16_ri(REG0,1,DEFAULT);
    // REG0 for & rr_side+(i3*i3-1)/2
    VoC_add16_rd(REG0, REG0, C1035PF_RR_SIDE_ADDR_ADDR);

    // REG0 for & rr_side+i3*(i3-1)/2+i2
    VoC_add16_rr(REG0,REG0,REG2,DEFAULT);
    // REG2 for i3*5
    VoC_mult16_rr(REG2,REG4,REG5,DEFAULT);

    // REG4 for rr_side[i2+5][i3] (rr[i2+5][i3])
    VoC_lw16_p(REG4, REG0,DEFAULT);
    // REG2 for i3*5+10
    VoC_add16_rd(REG2,REG2,CONST_10_ADDR);


    // REG0 for &rr_side[i2+10][i3]
    VoC_add16_rr(REG0, REG0, REG2,DEFAULT);
    // continue loop to 8 times

    VoC_loop_r_short(REG1,DEFAULT)
    // REG2 for 5*i3+10
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16_p(REG4, REG0,DEFAULT);
    VoC_add16_rr(REG0, REG0, REG2,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw16inc_p(REG4, REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    exit_on_warnings=ON;
    VoC_endloop0;

SEARCH_COPY_label_end:

///copy the  rr[i2][]  to  cp_rr[][7] , and use  cp_rr[][7] to address  rr[i2][i3] end


    //alp1
    VoC_pop16(REG5,DEFAULT);

    VoC_lw32_d(REG01,SEARCH_CODE_DN_RRV_ADDR_ADDR); //reg01:&dn[i3] and &rrv[i3]

    VoC_add16_pd(REG3,REG5,SEARCH_10I40_PS0_ADDR);  //REG3:ps1

    VoC_lw16i_set_inc(REG2, SEARCH_CODE_CP_RR_ADDR+7,8);       //  &rr[i2][i3]   at   CP_RR



    VoC_loop_i_short(8,DEFAULT)
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_startloop0

    VoC_mac32_pd(REG1,SEARCH_CODE_RRV_COE_ADDR);
    VoC_mac32inc_pd(REG2,SEARCH_CODE_RR_COE_ADDR);

    VoC_add16inc_rp(REG4,REG3,REG0,DEFAULT);
    VoC_movreg16(RL6_HI,REG4,DEFAULT);

    VoC_multf16_rr(REG4,REG4,REG4,DEFAULT);
    VoC_movreg16(REG5,ACC0_HI,DEFAULT);

    VoC_multf32_rr(ACC1,REG6,REG5,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG4,IN_PARALLEL);
    VoC_NOP();

    VoC_bngt32_rr(SEARCH_CODE_I3_END,ACC0,ACC1)
    VoC_push16(REG1,DEFAULT);
    VoC_lw16i(REG7,SEARCH_10I40_RRV_ADDR);
    VoC_sub16_rr(REG1,REG1,REG7,DEFAULT);
    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg16(RL7_HI,REG1,IN_PARALLEL);
    VoC_pop16(REG1,DEFAULT);
    VoC_lw16_d(RL7_LO,SEARCH_10I40_I2_TEMP_ADDR);
    VoC_movreg16(RL6_LO,RL6_HI, DEFAULT);

SEARCH_CODE_I3_END:;

    VoC_inc_p(REG1,DEFAULT);
    VoC_lw32_sd(ACC0,0,IN_PARALLEL);
    VoC_endloop0

    VoC_pop32(ACC1,DEFAULT);

    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16i_short(INC1,-1,IN_PARALLEL);
    VoC_add16_rd(REG2, REG2,SEARCH_10I40_I2_TEMP_ADDR);

    VoC_endloop1

    VoC_multf32_rd(ACC0,REG7,CONST_0x4000_ADDR);
    VoC_pop32(REG01,DEFAULT);
    VoC_add32_rd(ACC0,ACC0,CONST_0x00008000_ADDR);

    //RL6_LO:ps;rl_7:ia and ib;acc0:new alp0+0x8000
    VoC_return

}




/*******************************************************
  Function: CII_q_gain_code
  INPUT: REG0:    frac_gcode0
         REG1:     exp_gcode0
 OUTPUT: REG6  qua_ener
         REG7  qua_ener_MR122
 return:  REG3
 Note: Because this function only work in mode 122 , delete the branch that not work in mode 122
 Optimized by Kenneth 10/14/2004
*******************************************************/
void CII_q_gain_code (void)
{
//stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,RA,RA
    VoC_push16(RA, DEFAULT);
    VoC_jal(CII_Pow2);      //REG2     gcode0 = extract_l (Pow2 (exp_gcode0, frac_gcode0));
    VoC_lw16_d(REG6, COD_AMR_GAIN_CODE_ADDRESS);

    VoC_shr16_ri(REG6, 1, DEFAULT);     //REG6  g_q0

    VoC_shr16_ri(REG2,-4, DEFAULT); //   REG2     gcode0= shr (*gain, 1);

    VoC_lw16i_set_inc(REG0, STATIC_CONST_qua_gain_code_ADDR, 3);
    VoC_lw16i(REG5,0x7FFF);
    VoC_lw16i_set_inc(REG3,0, 1);

    VoC_lw32z(RL6,DEFAULT);
    VoC_multf16inc_rp(REG4, REG2, REG0, IN_PARALLEL);
    VoC_loop_i_short(32, DEFAULT);     //      for (i = 1; i < NB_QUA_CODE; i++)     #define NB_QUA_CODE 32
    VoC_startloop0
    VoC_sub16_rr(REG1 , REG6, REG4, DEFAULT); //REG1 :   err
    VoC_bnltz16_r(q_gain_code_L140, REG1)
    VoC_sub16_dr(REG1 , CONST_0_ADDR, REG1);   //abs
q_gain_code_L140:
    VoC_bngt16_rr(q_gain_code_L150, REG5, REG1)
    VoC_movreg16(REG5, REG1,DEFAULT);
    VoC_movreg16(RL6_LO, REG3,IN_PARALLEL);    //RL6_LO   index
q_gain_code_L150:
    VoC_multf16inc_rp(REG4, REG2, REG0, DEFAULT);
    VoC_inc_p(REG3,DEFAULT);
    VoC_endloop0

    VoC_movreg16(REG3,RL6_LO, DEFAULT);    //REG3   index

    VoC_mult16_rd(REG4, REG3, CONST_3_ADDR);
    VoC_lw16i_set_inc(REG0, STATIC_CONST_qua_gain_code_ADDR, 1);
    VoC_add16_rr(REG0, REG4,REG0,DEFAULT );
    VoC_lw16_sd(REG1, 2,DEFAULT);   //COD_AMR_ANAP_ADDR

    VoC_multf16inc_rp(REG4, REG2, REG0, DEFAULT);

    VoC_sw16inc_p(REG3, REG1, DEFAULT);
    VoC_sw16_sd(REG1, 2,DEFAULT);   //COD_AMR_ANAP_ADDR


    VoC_shr16_ri(REG4,-1, DEFAULT);


    VoC_lw16inc_p(REG7, REG0, DEFAULT);//REG6  qua_ener
    VoC_lw16inc_p(REG6, REG0, DEFAULT);// REG7  qua_ener_MR122
    VoC_pop16(RA, DEFAULT);
    VoC_sw16_d(REG4, COD_AMR_GAIN_CODE_ADDRESS);
    VoC_return;
}


void CII_lpc_M122(void)
{
    VoC_push16(RA,DEFAULT);
    /* Autocorrelations */
    VoC_lw16i(REG6,COD_AMR_A_T_ADDRESS+11);
    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_P_WINDOW_12K2_ADDR,1);//p_window_mid address

    VoC_lw16i_set_inc(REG1,TABLE_WINDOW_160_80_ADDR,1);//window_160_80 address
    VoC_push16(REG0,DEFAULT);
    VoC_sw16_d(REG6,PARA4_ADDR_ADDR);/*for calling Levinson*/
    VoC_jal(CII_Autocorr);
    /* Lag windowing    */
    VoC_jal(CII_Lag_window);
    /* Levinson Durbin  */
    VoC_jal(CII_Levinson);

    /* Autocorrelations */
    VoC_lw16i(REG6,COD_AMR_A_T_ADDRESS+33);
    VoC_pop16(REG0,DEFAULT);

    VoC_lw16i_set_inc(REG1,TABLE_WINDOW_232_8_ADDR,1);//window_160_80 address
    VoC_sw16_d(REG6,PARA4_ADDR_ADDR);/*for calling Levinson*/
    VoC_jal(CII_Autocorr);
    /* Lag windowing    */
    VoC_jal(CII_Lag_window);
    /* Levinson Durbin  */
    VoC_jal(CII_Levinson);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG0,1,DEFAULT);
    VoC_return

}
/***************************************
  Function CII_lsp_M122
  input:
     &lsp_new_q[]/&st->lsp_old_q[]   : push32(n+3)
    &az[]/&azQ[]            : push32(n+2)
    &anap[]/&st->qSt->past_rq[] : push32(n+1)
    &lsp_new[]          : push16(n+2)
    &st->lsp_old[]          : push16(n+1)
  Optimized by Kenneth   09/20/2004
***************************************/
void CII_lsp_M122(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG1,LSP_LSP_MID_ADDR,-1);
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_push16(REG1,DEFAULT);   // push16(n-1)  &lsp_mid[]
    VoC_jal(CII_Az_lsp);

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_lw16i_short(REG6,22,IN_PARALLEL);
    VoC_lw16_sd(REG1,3,DEFAULT);
    VoC_jal(CII_Az_lsp);


    //Int_lpc_1and3_2 (st->lsp_old, lsp_mid, lsp_new, az);
    VoC_lw16_sd(REG2,0,DEFAULT);
    VoC_lw32_sd(REG01,1,IN_PARALLEL);
    VoC_lw16_sd(REG3,2,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);
    VoC_lw16_sd(REG5,3,DEFAULT);
    VoC_movreg16(REG0,REG1,IN_PARALLEL);
    VoC_jal(CII_Int_lpc_1and3);



    VoC_lw16i(REG6,LSP_LSP_MID_Q_ADDR);
    VoC_lw16_sd(REG0,0,DEFAULT);        // load &lsp_mid[]
    VoC_lw32_sd(REG23,2,IN_PARALLEL);   // load reg3->&lsp_new_q[]
    VoC_push16(REG6,DEFAULT);       // push16(n-2) lsp_mid_q
    VoC_jal(CII_Q_plsf_5);

    VoC_lw32_sd(REG45,0,DEFAULT);
    VoC_lw16i_short(REG7,5,IN_PARALLEL);
    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);   //  (*anap) += add(0,5);

    //Int_lpc_1and3 (st->lsp_old_q, lsp_mid_q, lsp_new_q, azQ);
    VoC_lw32_sd(REG01,1,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);
    VoC_sw16_sd(REG4,6,DEFAULT);  //COD_AMR_ANAP_ADDR

    VoC_lw32_sd(REG45,2,DEFAULT);   // &lsp_new_q[]/&st->lsp_old_q[]
    VoC_pop16(REG2,DEFAULT);    // pop16(n-2)   lsp_mid_q
    VoC_movreg16(REG3,REG4,IN_PARALLEL);
//  VoC_bez16_d(lsp_M122_DTX_L0,GLOBAL_EFR_FLAG_ADDR);
//  VoC_bez16_d(lsp_M122_DTX_L1,GLOBAL_OUTPUT_SP_ADDR)
//lsp_M122_DTX_L0:
    VoC_jal(CII_Int_lpc_1and3);
//lsp_M122_DTX_L1:
    VoC_pop16(REG1,DEFAULT);    // pop16(n-1)  &lsp_mid[]
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_return;
}

/*``````````````````````````````````````````````````````
PARAMETERS:

REG0:&corr_ptr[-lag_max]    INC 2
REG1:lag_max                INC -1
REG4:number of loops ;
REG5:L_frame/2

OUTPUT: REG7:cor_max
RETURN: REG6:p_max

````````````````````````````````````````````````````````*/

void CII_Lag_max_M122(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw32_d(RL6,CONST_0x80000000_ADDR);      //RL6 for max
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_movreg16(REG6,REG1,IN_PARALLEL);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_bgt32_rr(LAG_MAX_M122_02,RL6,RL7)
    VoC_movreg32(RL6,RL7,DEFAULT);
    VoC_movreg16(REG6,REG1,IN_PARALLEL);//REG6:p_max;
LAG_MAX_M122_02:;
    VoC_lw32inc_p(RL7,REG0,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);                        //i--
    VoC_endloop0


    /*return p_max*/
    VoC_push16(REG6,DEFAULT);

    /* compute energy */
    VoC_lw16i_set_inc(REG0,AMR_LOCAL_VARS_Y_LEVEL1_START_ADDR+143,2);
    VoC_sub16_rr(REG0,REG0,REG6,DEFAULT);           //&scal_sig[-p_max]
    VoC_lw32z(ACC0,DEFAULT);

    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_loop_r_short(REG5,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0

    /* 1/sqrt(energy) */
//  VoC_bez16_d(LAG_MAX_M122__03,GLOBAL_DTX_ADDR)
//      /* check tone */
//      //RL6:max ACC0:t0
//      VoC_add32_rd(REG67,ACC0,CONST_0x00008000_ADDR);
//      VoC_bngtz16_r(LAG_MAX_M122__03,REG7)
//      VoC_lw16i(REG6,21298);
//      VoC_multf32_rr(REG67,REG7,REG6,DEFAULT);
//
//      VoC_lw16i(REG0,0x4000);
//      VoC_or16_rd(REG0,STRUCT_VADSTATE1_TONE_ADDR);
//      VoC_bngt32_rr(LAG_MAX_M122__03,RL6,REG67)
////        VoC_NOP();
//          VoC_sw16_d(REG0,STRUCT_VADSTATE1_TONE_ADDR);
//
//LAG_MAX_M122__03:
    VoC_NOP();
    VoC_movreg32(REG01,ACC0,DEFAULT);               //L_x

    VoC_jal(CII_Inv_sqrt);
    VoC_shr32_ri(REG01,-1,DEFAULT);

    /* max = max/sqrt(energy)  */
    VoC_movreg32(REG45,RL6,DEFAULT);
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG0,1,IN_PARALLEL);
//  VoC_and16_ri(REG4,0x7fff);
//  VoC_and16_ri(REG0,0x7FFF);


    VoC_multf32_rr(ACC0,REG5,REG1,DEFAULT);
    VoC_multf16_rr(REG6,REG5,REG0,IN_PARALLEL);

    VoC_multf16_rr(REG4,REG4,REG1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);
//  VoC_mac32_rd(REG6,CONST_1_ADDR);
//  VoC_mac32_rd(REG4,CONST_1_ADDR);
    VoC_mac32_rr(REG6,REG7,DEFAULT);
    VoC_mac32_rr(REG4,REG7,DEFAULT);

    VoC_lw16_d(REG7,PITCH_OL_M122_SCAL_FAC_ADDR);
    VoC_shr32_rr(ACC0,REG7,DEFAULT);
    VoC_shr32_ri(ACC0,-15,DEFAULT);
    VoC_pop16(REG6,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG7,ACC0_HI,DEFAULT);
    VoC_return;

}

/*
RETURN:REG4:p_max1
optimize by stephen 1020
*/
void CII_Pitch_ol_M122(void)
{

    VoC_lw32z(ACC0,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_sw32_d(ACC0,STRUCT_COD_AMRSTATE_OL_GAIN_FLG_ADDR);

//  VoC_lw16_d(REG4,STRUCT_VADSTATE1_TONE_ADDR);        //st->tone
//  VoC_bez16_d(PITCH_OL_NOT_DTX,GLOBAL_DTX_ADDR)
//          VoC_shr16_ri(REG4,1,DEFAULT);
//
//PITCH_OL_NOT_DTX:

    VoC_lw16i_set_inc(REG0,STRUCT_COD_AMRSTATE_OLD_WSP_ADDR-1,2);
//  VoC_sw16_d(REG4,STRUCT_VADSTATE1_TONE_ADDR);    //store st->tone

    VoC_add16_rd(REG0,REG0,COD_AMR_I_SUBFR_ADDRESS);
    VoC_add16_rd(REG5,REG0,CONST_1_ADDR);                       //&(old_wsp)

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_multf32_rr(ACC0,REG7,REG7,DEFAULT);

    VoC_loop_i(0,(PITCH_OL_M122_LOOP_NUM-1)/2)
    VoC_bimac32inc_pp(REG0,REG0);
    VoC_endloop0

    /*--------------------------------------------------------*
     * Scaling of input signal.                               *
     *                                                        *
     *   if Overflow        -> scal_sig[i] = signal[i]>>3     *
     *   else if t0 < 1^20  -> scal_sig[i] = signal[i]<<3     *
     *   else               -> scal_sig[i] = signal[i]        *
     *--------------------------------------------------------*/

    /*--------------------------------------------------------*
     *  Verification for risk of overflow.                    *
     *--------------------------------------------------------*/
    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_lw16i_short(REG3,PITCH_OL_M122_LOOP_NUM/4+1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL_ADDR,2);


    VoC_aligninc_pp(REG0,REG0,DEFAULT);
    VoC_bne32_rd(PITCH_OL_122_01,ACC0,CONST_0x7FFFFFFF_ADDR)
    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_jump(PITCH_OL_122_SCALE_END)

PITCH_OL_122_01:
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,0x10,IN_PARALLEL); //1048576
    VoC_bngt32_rr(PITCH_OL_122_02,REG45,ACC0)
    VoC_lw16i_short(REG2,-3,DEFAULT);
    VoC_jump(PITCH_OL_122_SCALE_END)
PITCH_OL_122_02:
    VoC_lw16i_short(REG2,0,DEFAULT);

PITCH_OL_122_SCALE_END:
    VoC_lw16i(REG6,56);
    VoC_jal(CII_scale);
    /* calculate all coreelations of scal_sig, from pit_min to pit_max */
    /*copy scaled_signal to RAM_X*/
    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_SCALED_SIGNAL_ADDR,2);
    VoC_sw16_d(REG2,PITCH_OL_M122_SCAL_FAC_ADDR);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR,2);

    VoC_jal(CII_copy_xy);

    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_SCALED_SIGNAL_ADDR+143,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR,2);
    VoC_lw16i_set_inc(REG2,PITCH_OL_M122_CORR_ADDR-2,2);
// begin of CII_comp_corr
    VoC_movreg16(REG6,REG0,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_add16_rr(REG7,REG1,REG3,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
//  exit_on_odd_address = OFF;

    VoC_loop_i(1,126)
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_movreg32(REG01,REG67,DEFAULT);
    VoC_sw32inc_p(RL6,REG2,IN_PARALLEL);
    VoC_movreg32(RL6,ACC0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG3,DEFAULT);
    VoC_endloop1
    VoC_sw32_p(RL6,REG2,DEFAULT);
//  exit_on_odd_address = ON;
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
// end of CII_comp_corr

    /*--------------------------------------------------------------------*
     *  The pitch lag search is divided in three sections.                *
     *  Each section cannot have a pitch multiple.                        *
     *  We find a maximum for each section.                               *
     *  We compare the maximum of each section by favoring small lags.    *
     *                                                                    *
     *  First section:  lag delay = pit_max     downto 4*pit_min          *
     *  Second section: lag delay = 4*pit_min-1 downto 2*pit_min          *
     *  Third section:  lag delay = 2*pit_min-1 downto pit_min            *
     *--------------------------------------------------------------------*/
    /*
        j = shl (pit_min, 2);
        p_max1 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          pit_max, j, &max1, dtx);
    */
    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_CORR_ADDR,2);
    VoC_lw16i_set_inc(REG1,143,-1);
    VoC_lw16i(REG4,72);

    VoC_jal(CII_Lag_max_M122);
    /*
        p_max2 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          i, j, &max2, dtx);

        */
    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_CORR_ADDR+144,2);
    VoC_sw32_d(REG67,PITCH_OL_M122_P_MAX1_ADDR);
    VoC_lw16i_set_inc(REG1,71,-1);
    VoC_lw16i(REG4,36);
    VoC_lw16i(REG5,40);
    VoC_jal(CII_Lag_max_M122);

    /*
        p_max3 = Lag_max (vadSt, corr_ptr, scal_sig, scal_fac, scal_flag, L_frame,
                          i, pit_min, &max3, dtx);

        */

    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_CORR_ADDR+216,2);
    VoC_sw32_d(REG67,PITCH_OL_M122_P_MAX2_ADDR);
    VoC_lw16i_set_inc(REG1,35,-1);
    VoC_lw16i_short(REG4,18,DEFAULT);
    VoC_lw16i_short(REG5,40,IN_PARALLEL);
    VoC_jal(CII_Lag_max_M122);
    VoC_sw32_d(REG67,PITCH_OL_M122_P_MAX3_ADDR);

//  VoC_bez16_d(PITCH_OL_M122_04a,GLOBAL_DTX_ADDR)
//  VoC_bnez16_d(PITCH_OL_M122_04b,COD_AMR_SUBFRNR_ADDRESS)
//       /* calculate max high-passed filtered correlation of all lags */
//PITCH_OL_M122_04a:
    VoC_jump(PITCH_OL_M122_04);
//PITCH_OL_M122_04b:
    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_CORR_ADDR+0,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_CORR_ADDR+2,2);
    VoC_lw16i_set_inc(REG2,PITCH_OL_M122_CORR_ADDR+4,2);

// begin of CII_hp_max
    VoC_lw32_d(ACC0,CONST_0x80000000_ADDR); //max
    VoC_lw32inc_p(RL6,REG1,DEFAULT);

    VoC_loop_i(0,122)
    VoC_shr32_ri(RL6,-1,DEFAULT);
    VoC_sub32inc_rp(RL6,RL6,REG0,DEFAULT);
    VoC_sub32inc_rp(RL6,RL6,REG2,DEFAULT);
    VoC_bnltz32_r(HP_MAX_01,RL6)
    VoC_sub32_dr(RL6,CONST_0_ADDR,RL6);
HP_MAX_01:
    VoC_bgt32_rr(HP_MAX_02,ACC0,RL6)
    VoC_movreg32(ACC0,RL6,DEFAULT);
HP_MAX_02:
    VoC_lw32inc_p(RL6,REG1,DEFAULT);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_rd(REG3,REG1,CONST_1_ADDR);   //shift1
    VoC_lw16i_set_inc(REG0,PITCH_OL_M122_SCALED_SIGNAL_ADDR+143,2);
    VoC_lw16i_set_inc(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR+143,2);
    VoC_shr32_ri(ACC0,1,DEFAULT);
    VoC_movreg16(REG4,ACC0_HI,DEFAULT);         //max16
    /* compute energy */

//  exit_on_odd_address = OFF;
    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0

    VoC_lw16i(REG0,PITCH_OL_M122_SCALED_SIGNAL_ADDR+143);
    VoC_lw16i(REG1,PITCH_OL_M122_SCALED_SIGNAL2_ADDR+143-1);
    VoC_movreg32(RL6,ACC0,DEFAULT); //t0

    VoC_aligninc_pp(REG0,REG1,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_loop_i_short(40,DEFAULT)
    VoC_startloop0
    VoC_bimac32inc_pp(REG0,REG1);
    VoC_endloop0
//  exit_on_odd_address = ON;

    /* high-pass filtering */
    VoC_shr32_ri(RL6,-1,DEFAULT);
    VoC_shr32_ri(ACC0,-1,DEFAULT);
    VoC_sub32_rr(ACC0,RL6,ACC0,DEFAULT);
    VoC_bnltz32_r(HP_MAX_03,ACC0)
    VoC_sub32_dr(ACC0,CONST_0_ADDR,ACC0);
HP_MAX_03:
    /* max/t0 */
    VoC_jal(CII_NORM_L_ACC0);
    VoC_sub16_rr(REG7,REG3,REG1,DEFAULT);   //shift
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL); //t016
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);        //cor_max
    VoC_bez16_r(HP_MAX_04,REG1)
    VoC_jal(CII_DIV_S);
HP_MAX_04:
    VoC_shr16_rr(REG2,REG7,DEFAULT);
    VoC_NOP();
// end of CII_hp_max
    /* update complex background detector */
//  VoC_sw16_d(REG2,STRUCT_VADSTATE1_BEST_CORR_HP_ADDR);

PITCH_OL_M122_04:
    /*--------------------------------------------------------------------*
     * Compare the 3 sections maximum, and favor small lag.               *
     *--------------------------------------------------------------------*/

    VoC_lw16i(REG6,27853);
    VoC_lw32_d(REG45,PITCH_OL_M122_P_MAX1_ADDR);
    VoC_multf16_rr( REG7,REG6,REG5,DEFAULT);
    VoC_pop16(RA,DEFAULT);
    VoC_bnlt16_rd(PITCH_OL_M122_05,REG7,PITCH_OL_M122_MAX2_ADDR)
    VoC_lw32_d(REG45,PITCH_OL_M122_P_MAX2_ADDR);

PITCH_OL_M122_05:
    VoC_multf16_rr( REG7,REG6,REG5,DEFAULT);
    VoC_NOP();
    VoC_bnlt16_rd(PITCH_OL_M122_06,REG7,PITCH_OL_M122_MAX3_ADDR)
    VoC_lw16_d(REG4,PITCH_OL_M122_P_MAX3_ADDR);

PITCH_OL_M122_06:
    VoC_return


}

/*********************************************
 Function:  CII_Q_plsf_5
 Input:
       st->past_rq/*indice  :push32(n)
       *lsp1(&lsp_mid[])    :REG0
       *lsp2(&lsp_new[])    :push16(n+4)
       *lsp1_q(&lsp_mid_q[])    :push16(n+1)
       *lsp2_q(&lsp_new_q[])    :REG3
 Optimized by Kenneth 09/18/2004
**********************************************/
void CII_Q_plsf_5(void)
{
    VoC_push16(RA,DEFAULT );
    VoC_lw16i_set_inc(REG1,Q_PLSF_5_LSF1_ADDR,-1);
    VoC_push16(REG3,DEFAULT);       // push16(n-1) *lsp2_q(&lsp_new_q[])
    VoC_lw16i_short(INC0,-1,IN_PARALLEL);
    VoC_push16(REG1,DEFAULT);       // push16(n-2) &lsf1[]

    VoC_jal(CII_Lsp_lsf);           // Lsp_lsf (lsp1, lsf1, M);

    VoC_lw16i_set_inc(REG1,Q_PLSF_5_LSF2_ADDR,-1);
    VoC_lw16_sd(REG0,7,DEFAULT);        // load &lsp2[]
    VoC_push16(REG1,DEFAULT);       // push16(n-3) &lsf2[]
    VoC_jal(CII_Lsp_lsf);           // Lsp_lsf (lsp2, lsf2, M);



    VoC_lw16i_set_inc(REG0,Q_PLSF_5_LSF_R1_ADDR,1);
    VoC_lw16i_set_inc(REG1,Q_PLSF_5_WF1_ADDR,1);
    VoC_lw16i(REG4,Q_PLSF_5_LSF_R2_ADDR );
    VoC_lw16i(REG5,Q_PLSF_5_WF2_ADDR );
    VoC_push32(REG01,DEFAULT);      // push32(n-1)  wf1/lsf_r1
    VoC_push32(REG45,DEFAULT);      // push32(n-2)  wf2/lsf_r2

    VoC_lw16_sd(REG0,1,DEFAULT);
    VoC_jal(CII_Lsf_wt);            //Lsf_wt (lsf1, wf1);

    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Lsf_wt);            //Lsf_wt (lsf2, wf2);
//Q_PLSF_5_05:

    VoC_lw32_sd(REG01,2,DEFAULT);
    VoC_lw16i_short(INC1,2,IN_PARALLEL);   //REG1 addr of past_rq[i]
    VoC_lw16i_set_inc(REG0,TABLE_MEAN_LSF_ADDR,2);  //REG0 addr of mean_lsf[i]
    VoC_lw16i_set_inc(REG3, Q_PLSF_5_LSF_P_ADDR,2); //REG2 addr of lsp[i]
    VoC_lw16i_set_inc(REG2, 21299,2);               //REG3 for const PRED_FAC

    VoC_multf16inc_rp(REG6,REG2,REG1, DEFAULT);         // for (i = 0; i < M; i++)
    VoC_multf16inc_rp(REG7,REG2,REG1, IN_PARALLEL);         //    {
    VoC_loop_i_short((M/2),DEFAULT)                         //        lsf_p[i] = add (mean_lsf_5[i], mult (st->past_rq[i], LSP_PRED_FAC_MR122));
    VoC_startloop0                                          //        move16 ();
    VoC_add16inc_rp(REG4,REG6,REG0, DEFAULT);       //        lsf_r1[i] = sub (lsf1[i], lsf_p[i]);           move16 ();
    VoC_add16inc_rp(REG5,REG7,REG0, IN_PARALLEL);   //        lsf_r2[i] = sub (lsf2[i], lsf_p[i]);           move16 ();
    VoC_multf16inc_rp(REG6,REG2,REG1, DEFAULT);     //    }
    VoC_multf16inc_rp(REG7,REG2,REG1, IN_PARALLEL);
    VoC_sw32inc_p(REG45,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG3,Q_PLSF_5_LSF_P_ADDR,2);
    VoC_pop16(REG2,DEFAULT);    //pop16(n-3)     REG2:&lsf2[i]
    VoC_pop16(REG0,DEFAULT);    //pop16(n-2)     REG0:&lsf1[i]
    VoC_movreg16(REG1,REG3,IN_PARALLEL);                //REG1:&lsf_p
    VoC_lw16i(REG4, Q_PLSF_5_LSF_R1_ADDR);  //REG4 &lsf_r1[i]
    VoC_lw16i(REG5, Q_PLSF_5_LSF_R2_ADDR);  //REG4 &lsf_r1[i]

    VoC_lw16i_short(RL6_HI,2,DEFAULT);
    VoC_lw16i_short(RL6_LO,2,IN_PARALLEL);

    VoC_loop_i_short((M/2),DEFAULT);
    VoC_startloop0
    VoC_sub16inc_pp(REG6,REG0,REG1,DEFAULT);
    VoC_sub16inc_pp(REG7,REG0,REG1,IN_PARALLEL);

    VoC_sub16inc_pp(REG6,REG2,REG3,DEFAULT);
    VoC_sub16inc_pp(REG7,REG2,REG3,IN_PARALLEL);
    exit_on_warnings=OFF;
    VoC_sw32_p(REG67,REG4,DEFAULT);
    exit_on_warnings=ON;
    VoC_sw32_p(REG67,REG5,DEFAULT);
    VoC_add32_rr(REG45,REG45,RL6,IN_PARALLEL);
    VoC_endloop0
//Q_PLSF_5_06:
    VoC_lw32z(RL7,DEFAULT);
    VoC_lw16i_short(RL6_HI,2,DEFAULT);
    VoC_lw16i_short(RL6_LO,2,IN_PARALLEL);

    VoC_lw32_sd(REG23,2,DEFAULT);
    VoC_lw16i_short(REG6,4,IN_PARALLEL);        // 128/32=4
    VoC_lw16i_set_inc(REG3,TABLE_DICO1_LSF_ADDR,2);
    VoC_push16(REG2,DEFAULT);           // push16(n-2) &indice[]

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_jal(CII_Vq_subvec);             //indice[0] = Vq_subvec (&lsf_r1[0], &lsf_r2[0], dico1_lsf_5,&wf1[0], &wf2[0], DICO1_SIZE_5);
    VoC_lw16i_set_inc(REG3,TABLE_DICO2_LSF_ADDR,2);
    VoC_lw16i_short(REG6,8,DEFAULT);        // 256/32=8
    VoC_jal(CII_Vq_subvec);             //indice[1] = Vq_subvec (&lsf_r1[2], &lsf_r2[2], dico2_lsf_5,&wf1[2], &wf2[2], DICO2_SIZE_5);
    VoC_lw16i_set_inc(REG3,TABLE_DICO3_LSF_ADDR,2);
    VoC_lw16i_short(REG6,8,DEFAULT);        // 256/32=8
    VoC_lw16i_short(RL7_LO,1,DEFAULT);
    VoC_jal(CII_Vq_subvec);             //indice[2] = Vq_subvec_s (&lsf_r1[4], &lsf_r2[4], dico3_lsf_5,&wf1[4], &wf2[4], DICO3_SIZE_5);
    VoC_lw16i_set_inc(REG3,TABLE_DICO4_LSF_ADDR,2);
    VoC_lw16i_short(REG6,8,DEFAULT);        //256/32=8
    VoC_lw32z(RL7,IN_PARALLEL);
    VoC_jal(CII_Vq_subvec);             //indice[3] = Vq_subvec (&lsf_r1[6], &lsf_r2[6], dico4_lsf_5,&wf1[6], &wf2[6], DICO4_SIZE_5);
    VoC_lw16i_set_inc(REG3,TABLE_DICO5_LSF_ADDR,2);
    VoC_lw16i_short(REG6,2,DEFAULT);        // 64/32=2
    VoC_jal(CII_Vq_subvec);             //indice[4] = Vq_subvec (&lsf_r1[8], &lsf_r2[8], dico5_lsf_5,&wf1[8], &wf2[8], DICO5_SIZE_5);


    VoC_pop16(REG2,DEFAULT);            // pop16(n-2) &indice[]

    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1, Q_PLSF_5_LSF2_Q_ADDR,1);//REG1 addr of lsf2_q[i]
    VoC_lw16i_set_inc(REG0, Q_PLSF_5_LSF1_Q_ADDR,1);//REG0 addr of lsf1_q[i]
    VoC_push16(REG1,DEFAULT);           // push16(n-2)  lsf2_q[i]
    VoC_pop32(REG45,IN_PARALLEL);           // pop32(n-2)  wf2/lsf_r2 , but here REG5->&lsf_r1[]
    VoC_push16(REG0,DEFAULT);           // push16(n-3)  lsf1_q[i]
    VoC_pop32(REG23,IN_PARALLEL);           // pop32(n-1)  wf1/lsf_r1 , but here REG2->&lsf_r2[]
    VoC_lw16i(REG4,Q_PLSF_5_LSF_P_ADDR);
    VoC_lw32_sd(REG67,0,DEFAULT);       //

    VoC_lw16i_short(INC3,1,IN_PARALLEL);
    VoC_loop_i_short(10,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);        //REG3: &st->past_rq[]
    // reg0:&lsf1[] reg2:&lsf_r2        reg4:&lsf_r1[]
    // reg1:&lsf2[] reg3:&st->past_rq[]     reg5:&lsf_p[]
    VoC_startloop0                  // for (i = 0; i < M; i++)
    VoC_add16_pp(REG6,REG4,REG5,DEFAULT);   //     {
    VoC_lw16inc_p(REG7,REG2,DEFAULT);           //         lsf1_q[i] = add (lsf_r1[i], lsf_p[i]);          move16 ();
    VoC_sw16inc_p(REG6,REG0,DEFAULT);           //         lsf2_q[i] = add (lsf_r2[i], lsf_p[i]);          move16 ();
    VoC_sw16inc_p(REG7,REG3,DEFAULT);           //         st->past_rq[i] = lsf_r2[i];                     move16 ();
    VoC_add16_rp(REG7,REG7,REG4,IN_PARALLEL);       // }
    VoC_add32_rd(REG45,REG45,CONST_0x00010001_ADDR);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16_sd(REG0,0,DEFAULT);
    VoC_jal(CII_Reorder_lsf);           // Reorder_lsf (lsf1_q, LSF_GAP, M);
    VoC_lw16_sd(REG0,1,DEFAULT);                // Reorder_lsf (lsf2_q, LSF_GAP, M);
    VoC_jal(CII_Reorder_lsf);


    VoC_pop16(REG0,DEFAULT);            // pop16(n-3)  &lsf1_q[i]
    VoC_lw16_sd(REG1,3,DEFAULT);            // load &lsp1_q[]
    VoC_jal(CII_Lsf_lsp);                   // Lsf_lsp (lsf1_q, lsp1_q, M);

    VoC_pop16(REG0,DEFAULT);            // pop16(n-2) &lsf2_q[i]
    VoC_pop16(REG1,DEFAULT);            // pop16(n-1) &lsp2_q []
    VoC_jal(CII_Lsf_lsp);               // Lsf_lsp (lsf2_q, lsp2_q, M);

//Q_PLSF_5_07:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}






/***********************************************
  Function CII_Int_lpc_1and3
  input: REG3->&lsp_old[]
         REG2->&lsp_mid[]
         REG5->&lsp_new[]
         REG0->&Az[]
         REG6->
         REG6=1 -->CII_Int_lpc_1and3
         REG6=0 -->CII_Int_lpc_1and3_2
         Optimised by Cui 2005.01.25
***********************************************/
void CII_Int_lpc_1and3(void)
{

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_set_inc(REG1,INT_LPC_LSP_ADDR,2);

Int_lpc_1and3_L0:
    VoC_bne16_rd(Int_lpc_1and3_L3,REG7,CONST_44_ADDR);
    VoC_return;
Int_lpc_1and3_L3:

    VoC_push16(RA,DEFAULT);                // push16(n)

    VoC_push16(REG5,DEFAULT);           // push16(n-1)  lsp_new
    VoC_push32(REG67,IN_PARALLEL);          // push32(n)    N/REG6
    VoC_push16(REG2,DEFAULT);           // push16(n-2)  lsp_mid
    VoC_push32(REG01,IN_PARALLEL);          // push32(n-1)  lsp/Az

    VoC_jal(CII_Int_lpc_only);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_jal(CII_Lsp_Az);
    VoC_lw32_sd(REG67,1,DEFAULT);
    VoC_add16_rd(REG7,REG7,CONST_22_ADDR);
    VoC_sub16_rd(REG5,REG7,CONST_11_ADDR);
    VoC_sw32_sd(REG67,1,DEFAULT);
    VoC_bez16_r(Int_lpc_1and3_L1,REG6);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);
    VoC_bne16_rd(Int_lpc_1and3_L2,REG5,CONST_33_ADDR);
    VoC_lw16_sd(REG1,1,DEFAULT);
Int_lpc_1and3_L2:
    VoC_jal(CII_Lsp_Az);
Int_lpc_1and3_L1:
    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);
    VoC_pop16(REG3,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_movreg16(REG5, REG3,IN_PARALLEL);
    VoC_jump(Int_lpc_1and3_L0);

    VoC_return;
}



void  CII_gainQuant_M122(void)
{

    // stack16:   mode,  xn2 ,st->h1, st->sharp,   gain_pit, res2,  code, y2, subfrNr, anap,RA
    VoC_push16(RA,DEFAULT);
    VoC_lw16i(REG1, STRUCT_GC_PREDST_ADDR);

    // inline CII_gc_pred_s1(void)
    VoC_lw16i_set_inc(REG0,COD_AMR_CODE_ADDRESS, 2);
    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_startloop0
    VoC_bimac32inc_pp(REG0, REG0);
    VoC_endloop0

    VoC_add16_rd(REG6, REG1, CONST_4_ADDR);
    VoC_add32_rd(REG23, ACC0, CONST_0x00008000_ADDR);
    VoC_multf32_rd(ACC0, REG3, CONST_26214_ADDR);
    VoC_jal(CII_Log2);  //   //REG4  exp   REG5   frac

//  VoC_lw32_d(ACC0, CONST_783741L_ADDR); //acc0 is     783741L    MEAN_ENER_MR122
    VoC_lw16i(ACC0_LO,0xf57d);
    VoC_movreg16(REG0, REG6,DEFAULT);          //REG0  :     st->past_qua_en_MR122
    VoC_lw16i_short(INC0,2,IN_PARALLEL);
    VoC_lw16i_set_inc(REG1, STATIC_CONST_pred_MR122_ADDR, 2);
    VoC_loop_i_short(2, DEFAULT);
    VoC_lw16i_short(ACC0_HI,0xb,IN_PARALLEL);
    VoC_startloop0;
    VoC_bimac32inc_pp(REG0, REG1);
    VoC_endloop0;

    VoC_movreg16(REG6, REG5, DEFAULT); //reg67 :ener_code
    VoC_shr32_ri(REG67, -1, DEFAULT);
    VoC_lw16i_short(REG2,30,IN_PARALLEL);

    VoC_sub16_rr(REG7, REG4, REG2,DEFAULT);
    VoC_sub32_rr(REG67, ACC0, REG67, DEFAULT);
    VoC_shr32_ri(REG67, 1, DEFAULT);
    VoC_shru16_ri(REG6, 1,DEFAULT);
//  VoC_and16_ri(REG6,0x7fff);

    VoC_lw16i_set_inc(REG0, COD_AMR_XN2_ADDRESS, 1);  //INPUT:   //      REG0       xn2   incr=1
    VoC_lw16i_set_inc(REG1, COD_AMR_Y2_ADDRESS, 2);     //   REG1      y2    incr=1

    // inline CII_G_code
    // INPUT:
    // REG0    xn2   incr=1
    // REG1    y2    incr=1
    // return  REG2

    VoC_lw32z(ACC0, DEFAULT);
    VoC_lw16i_short(ACC0_LO,1,DEFAULT);
    VoC_loop_i_short(20, DEFAULT);
    VoC_lw32z(ACC1, IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(REG23, REG1,DEFAULT);
    VoC_shr16_ri(REG2, 1, DEFAULT);
    VoC_shr16_ri(REG3, 1, IN_PARALLEL);
    VoC_mac32inc_rp(REG2, REG0, DEFAULT);
    VoC_mac32_rr(REG2, REG2, IN_PARALLEL);
    VoC_mac32inc_rp(REG3, REG0, DEFAULT);
    VoC_mac32_rr(REG3, REG3, IN_PARALLEL);
    VoC_endloop0

    VoC_jal(CII_NORM_L_ACC0);
    // REG1 exp_xy
    VoC_movreg32(REG23, ACC0, DEFAULT);
    //   REG3  xy
    VoC_bgtz16_r(G_code_go, REG3)
    VoC_lw16i_short(REG2, 0, DEFAULT);
    VoC_jump(CII_G_code01); // VoC_return;
G_code_go:

    VoC_add16_rd(REG4,  REG1,CONST_5_ADDR);
    // REG4   :    i = add (exp_xy, 5);
    VoC_movreg32(ACC0, ACC1, DEFAULT);
    VoC_jal(CII_NORM_L_ACC0);
    //REG1   -exp_yy

    // ACC0_HI    yy
    VoC_shr16_ri(REG3, 1, DEFAULT);
    VoC_sub16_rr(REG4, REG4, REG1, IN_PARALLEL);
    //REG4      i = sub (i, exp_yy);
    VoC_movreg16(REG0, REG3, DEFAULT);
    VoC_movreg16(REG1, ACC0_HI, IN_PARALLEL);
    VoC_jal(CII_DIV_S);
    //REG2 gain
    VoC_shr16_rr(REG2, REG4, DEFAULT);
//  VoC_bnez16_d(CII_G_code01, GLOBAL_EFR_FLAG_ADDR)
    VoC_shr16_ri(REG2, -1, DEFAULT);
CII_G_code01:

    // end inline
    // return REG2
    VoC_movreg32(REG01, REG67, DEFAULT);    //INPUT:       REG1:     exp_gcode0       REG0:    frac_gcode0
    VoC_sw16_d(REG2, COD_AMR_GAIN_CODE_ADDRESS);

    VoC_jal(CII_q_gain_code);
    //OUTPUT:     //REG6  qua_ener    REG7  qua_ener_MR122
    VoC_lw16i(REG5, STRUCT_GC_PREDST_ADDR);
    VoC_jal(CII_gc_pred_update);

    VoC_pop16(RA,DEFAULT);
    VoC_pop16(REG5, DEFAULT);
    VoC_return;
}


/***************************************
  Function CII_lsp
  Call it directly , has not any interface need to config
  Note:  CII_lsp includes CII_lsp_M475 and CII_lsp_M122
  Optimized by Kenneth  19/09/2004
***************************************/
void CII_lsp_122(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i(REG7,LSP_LSP_NEW_Q_ADDR);
    VoC_lw16i(REG6,STRUCT_LSPSTATE_LSP_OLD_Q_ADDR);
    VoC_lw16i_set_inc(REG1,COD_AMR_LSP_NEW_ADDRESS,-1);
    VoC_lw16i(REG4,COD_AMR_AQ_T_ADDRESS);
    VoC_lw16i(REG5,COD_AMR_A_T_ADDRESS);
    VoC_push16(REG1,DEFAULT);   // push16(n+2)   &lsp_new[]
    VoC_push32(REG67,IN_PARALLEL);  // push32(n+3)   &lsp_new_q[]/&st->lsp_old_q[]
    VoC_lw16i_set_inc(REG0,STRUCT_LSPSTATE_LSP_OLD_ADDR,1);
    VoC_lw16i(REG7,STRUCT_Q_PLSFSTATE_PAST_RQ_ADDR);
    VoC_lw16_sd(REG6,2,DEFAULT);
    VoC_push16(REG0,DEFAULT);   // push16(n+1)   &st->lsp_old[]
    VoC_push32(REG45,IN_PARALLEL);  // push32(n+2)   &az[]/&azQ[]
    VoC_push32(REG67,DEFAULT);  // push32(n+1)   &st->qSt->past_rq[]/&anap[]
    VoC_lw16i_short(REG3,7,IN_PARALLEL);

    VoC_jal(CII_lsp_M122);
    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);
    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_jal(CII_Copy_M);              // Copy (lsp_new, st->lsp_old, M);

//  VoC_bez16_d(efr_encoder103,GLOBAL_EFR_FLAG_ADDR)
//  VoC_bnez16_d(efr_encoder103,GLOBAL_OUTPUT_SP_ADDR)
//  VoC_lw16i_set_inc(REG0,COD_AMR_A_T_ADDRESS,2);
//  VoC_lw16i_set_inc(REG1,COD_AMR_AQ_T_ADDRESS,2);
//  VoC_lw16i_short(REG6,11,DEFAULT);
//    VoC_jal(CII_copy_xy);
//efr_encoder103:
    VoC_pop32(REG01,DEFAULT);
//  VoC_bez16_d(CII_lsp_122_1,GLOBAL_EFR_FLAG_ADDR)
//  VoC_bnez16_d(CII_lsp_122_1,GLOBAL_OUTPUT_SP_ADDR)
//  VoC_lw16i_set_inc(REG1,COD_AMR_LSP_NEW_ADDRESS,2);
//CII_lsp_122_1:
    VoC_pop16(RA,DEFAULT);
    VoC_loop_i_short(5,DEFAULT);
    VoC_lw32inc_p(ACC0,REG1,IN_PARALLEL);
    VoC_startloop0
    VoC_lw32inc_p(ACC0,REG1,DEFAULT);
    exit_on_warnings=OFF;
    VoC_sw32inc_p(ACC0,REG0,DEFAULT); //Copy (lsp_new_q, st->lsp_old_q, M);
    exit_on_warnings=ON;
    VoC_endloop0

    VoC_return;
}


//////////////////JPEG YUV PROCESS/////////////////////////////
//  VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,1);
//  VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
//  VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
//  VoC_push16(REG5,IN_PARALLEL);//width loop

void Convert_YUV422_Block(void)
{
    //r0:inbuf inc0=2
    //r1:y1  r2:y2  r3:u

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);

    VoC_movreg16(ACC0_LO,REG0,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);//
    VoC_movreg16(REG5,REG4,DEFAULT);//r5 = 64
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);//r4 = 128


    VoC_loop_i(1,8);//for (j=0;j<8;j++)
    //pyuv = (short*)buff + j*width;

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y

    VoC_lbinc_p(REG0);//temp = *pyuv++;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);//u
    VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,DEFAULT);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0
    VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
    //*py1++ = (temp&0xff)-128;
    //*pcb++ = ((temp>>8)&0xff)-128;
    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//y
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//u

    VoC_lbinc_p(REG0);//temp = *pyuv++;


    VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);

    VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
    VoC_sw16_p(REG7,REG3,IN_PARALLEL);//*pcr++ = ((temp>>8)&0xff)-128;


    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;

    VoC_add16_rr(REG6,REG3,REG5,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);//y
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);//v

    VoC_endloop0

    VoC_movreg16(REG0,ACC0_LO,DEFAULT);
    VoC_add16_rd(REG0,REG0,Input_JPEGENC_WIDTH_addr);
    VoC_movreg16(ACC0_LO,REG0,DEFAULT);

    VoC_endloop1

    VoC_return;
}

void  COOLSAND_YUVJPEG422(void)
{
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-4,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,7);
    VoC_sw16_d(REG5,Output_reserve0_addr);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_YUV422,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,Input_reserved_addr);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,Output_reserve0_addr);//whether the width has changed;
    VoC_sw16_d(REG4,Input_JPEGENC_WIDTH_addr);
LABLE_WIDTH_MULTIPLE16_YUV422:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV422,REG7);
    VoC_add16_rd(REG5,REG5,CONST_7_ADDR);
    VoC_and16_ri(REG5,0xfff8);
    VoC_NOP();
    VoC_sw16_d(REG5,Input_JPEGENC_HEIGHT_addr);
LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV422:
    VoC_lw16i_short(REG1,0,DEFAULT);

LABLE_REG2JPEG_LOOP_HEIGHT_YUV422:
    VoC_lw16i_short(REG0,-3,DEFAULT);//8=2**3
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_YUV422:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);


    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_jal(Convert_YUV422_Block);


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_YUV422,REG5,Input_JPEGENC_WIDTH_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_YUV422);
LABLE_CON1_YUV422:
    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_YUV422,REG1,Input_JPEGENC_HEIGHT_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_YUV422);
LABLE_CON2_YUV422:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}
//#if 0
//void Convert_YUV411_Block(void)
//{
//  //r0:inbuf inc0=2
//  //r1:y1  r2:y2  r3:u
//
//   VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
//   VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
//   VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
//
//   VoC_movreg16(ACC0_LO,REG0,DEFAULT);
//   VoC_lw16i_short(REG4,64,IN_PARALLEL);
//     VoC_lw32z(RL6,DEFAULT);
//     VoC_lw16i_short(REG6,2,IN_PARALLEL);
//   VoC_movreg16(REG5,REG4,DEFAULT);//r5 = 64
//   VoC_shru16_ri(REG4,-1,IN_PARALLEL);//r4 = 128
//
//
//
//LABLE_Convert_YUV411_L1:
//   VoC_push16(REG6,DEFAULT);
//   VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);//
//
//   VoC_loop_i(1,4);//for (j=0;j<4;j++)
//
//         //even row////////////////////////////////////////
//         VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
//         VoC_startloop0
//             VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
//             //*py1++ = (temp&0xff)-128;
//             //*pcb++ = ((temp>>8)&0xff)-128;
//             VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
//
//             VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
//             VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y
//
//             VoC_lbinc_p(REG0);//temp = *pyuv++;
//
//             VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);//u
//             VoC_sw16inc_p(REG6,REG1,IN_PARALLEL);
//
//               VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
//             VoC_sw16_p(REG7,REG3,DEFAULT);//*pcr++ = ((temp>>8)&0xff)-128;
//
//
//             VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
//               VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;
//
//             VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
//             VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);
//             VoC_sw16inc_p(REG6,REG1,DEFAULT);//y
//             VoC_inc_p(REG3,IN_PARALLEL);
//             VoC_sw16inc_p(REG7,REG6,DEFAULT);//v
//
//         VoC_endloop0
//
//         VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
//         VoC_startloop0
//             VoC_lbinc_p(REG0);//read a short ->rl6// temp = *pyuv++;
//             //*py1++ = (temp&0xff)-128;
//             //*pcb++ = ((temp>>8)&0xff)-128;
//             VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
//             VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
//
//             VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);//y
//             VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//u
//
//             VoC_lbinc_p(REG0);//temp = *pyuv++;
//
//
//             VoC_rbinc_i(REG6,8,DEFAULT);//take 8 bit from rl6
//             VoC_sw16inc_p(REG6,REG2,IN_PARALLEL);
//
//             VoC_rbinc_i(REG7,8,DEFAULT);//take 8 bit from rl6
//             VoC_sw16_p(REG7,REG3,IN_PARALLEL);//*pcr++ = ((temp>>8)&0xff)-128;
//
//
//             VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
//               VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//*py1++ = (temp&0xff)-128;
//
//             VoC_add16_rr(REG6,REG3,REG5,DEFAULT);
//             VoC_sw16inc_p(REG6,REG2,DEFAULT);//y
//             VoC_inc_p(REG3,IN_PARALLEL);
//             VoC_sw16inc_p(REG7,REG6,DEFAULT);//v
//
//         VoC_endloop0
//
//             VoC_movreg16(REG0,ACC0_LO,DEFAULT);
//             VoC_add16_rd(REG0,REG0,Input_JPEGENC_WIDTH_addr);//pyuv = (short*)buff + j*width;
//             VoC_movreg16(ACC0_LO,REG0,DEFAULT);
//          ///////odd row//////////////////////////////////
//         VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
//         VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
//         VoC_startloop0
//
//
//             VoC_shru16_ri(REG6,-8,DEFAULT);
//             VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);
//
//             VoC_shru16_ri(REG6,8,DEFAULT);
//             VoC_shru16_ri(REG7,-8,IN_PARALLEL);
//
//             VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
//             VoC_shru16_ri(REG7,8,IN_PARALLEL);
//
//
//               VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
//             VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
//
//             VoC_sw16inc_p(REG6,REG1,DEFAULT);
//             VoC_sw16inc_p(REG7,REG1,DEFAULT);
//         VoC_endloop0
//
//         VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
//         VoC_startloop0
//
//             VoC_shru16_ri(REG6,-8,DEFAULT);
//             VoC_lw16inc_p(REG7,REG0,IN_PARALLEL);
//
//             VoC_shru16_ri(REG6,8,DEFAULT);
//             VoC_shru16_ri(REG7,-8,IN_PARALLEL);
//
//             VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
//             VoC_shru16_ri(REG7,8,IN_PARALLEL);
//
//
//               VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
//             VoC_lw16inc_p(REG6,REG0,IN_PARALLEL);
//
//             VoC_sw16inc_p(REG6,REG2,DEFAULT);
//             VoC_sw16inc_p(REG7,REG2,DEFAULT);
//         VoC_endloop0
//
//             VoC_movreg16(REG0,ACC0_LO,DEFAULT);
//             VoC_add16_rd(REG0,REG0,Input_JPEGENC_WIDTH_addr);//pyuv = (short*)buff + j*width;
//             VoC_movreg16(ACC0_LO,REG0,DEFAULT);
//     VoC_endloop1
//
//             VoC_pop16(REG6,DEFAULT);
//           VoC_sub16_rd(REG6,REG6,CONST_1_ADDR);
//           VoC_bgt16_rd(LABLE_Convert_YUV411_L2,REG6,CONST_0_ADDR);
//             VoC_return;
//LABLE_Convert_YUV411_L2:
//           VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y2_ADDR,1);
//           VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y3_ADDR,1);
//             VoC_jump(LABLE_Convert_YUV411_L1);
//
//}
//#else
void Convert_YUV411_Block(void)
{
    //r0:inbuf inc0=2
    //r1:y1  r2:y2  r3:u

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,2);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,2);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);

    VoC_lw16i_short(REG5,16,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);
    VoC_sub16_dr(REG5,Input_JPEGENC_WIDTH_addr,REG5);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL);

    VoC_lw16i(ACC1_LO,0x00ff);//ACC1: 0X00FF00FF
    VoC_movreg16(ACC1_HI,ACC1_LO,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);//width - 16

    VoC_movreg16(REG5,REG4,DEFAULT);//r5 = 64
    VoC_shru16_ri(REG4,-1,IN_PARALLEL);//r4 = 128

LABLE_Convert_YUV411_L1:
    VoC_push16(REG6,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_loop_i(1,4);//for (j=0;j<4;j++)

    //even row////////////////////////////////////////
    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_lw32_p(REG67,REG0,IN_PARALLEL);//read a short ->rl6// temp = *pyuv++;
    VoC_startloop0

    VoC_and32_rr(REG67,ACC1,DEFAULT);//get y1 and y2
    VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);

    VoC_shru32_ri(RL7,8,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y1

    VoC_and32_rr(RL7,ACC1,DEFAULT);//u v
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);
    VoC_lw32_p(REG67,REG0,DEFAULT);

    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);//for (i=0;i<8;i+=2)
    VoC_startloop0

    VoC_and32_rr(REG67,ACC1,DEFAULT);//get y1 and y2
    VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);

    VoC_shru32_ri(RL7,8,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);//y1

    VoC_and32_rr(RL7,ACC1,DEFAULT);//u v
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);//

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG5,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG6,DEFAULT);
    VoC_lw32_p(REG67,REG0,DEFAULT);

    VoC_endloop0

    VoC_lw16_sd(REG6,1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_NOP();

    ///////odd row//////////////////////////////////
    //y2 part//
    VoC_lw32inc_p(REG67,REG0,DEFAULT);//1 2

    VoC_and32_rr(REG67,ACC1,DEFAULT);//y1 and y2
    VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);//3 4

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_and32_rr(RL7,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//5 6

    VoC_and32_rr(REG67,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//7  8

    VoC_and32_rr(REG67,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//1 2
    VoC_sw32inc_p(REG67,REG1,DEFAULT);

    //y3 part//
    VoC_and32_rr(REG67,ACC1,DEFAULT);
    VoC_lw32inc_p(RL7,REG0,IN_PARALLEL);//3 4

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_and32_rr(RL7,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//5 6

    VoC_and32_rr(REG67,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);//7  8

    VoC_and32_rr(REG67,ACC1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG4,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    //update r0
    VoC_lw16_sd(REG6,1,DEFAULT);
    VoC_sw32inc_p(REG67,REG2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);
    VoC_NOP();
    VoC_endloop1

    VoC_pop16(REG6,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);

    VoC_bgt16_rd(LABLE_Convert_YUV411_L2,REG6,CONST_0_ADDR);
    VoC_pop16(REG7,DEFAULT);
    VoC_return;
LABLE_Convert_YUV411_L2:
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y2_ADDR,2);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y3_ADDR,2);
    VoC_jump(LABLE_Convert_YUV411_L1);

}
// #endif

void  COOLSAND_YUV2JPEG411(void)
{

    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_push16(RA,DEFAULT);
    VoC_movreg32(REG67,REG45,IN_PARALLEL);
    VoC_shr16_ri(REG4,-5,DEFAULT);//w*32 byte
    VoC_lw16i_short(REG5,0,IN_PARALLEL);
    VoC_and16_ri(REG6,15);
    VoC_and16_ri(REG7,15);
    VoC_sw16_d(REG5,Output_reserve0_addr);//
    VoC_sw32_d(REG45,CONST_BLOCKSIZE_BYTE_ADDR);
    VoC_lw32_d(REG45,Input_JPEGENC_WIDTH_addr);//w h
    VoC_bez16_r(LABLE_WIDTH_MULTIPLE16_YUV,REG6);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_sw16_d(REG4,Input_reserved_addr);//original width;
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_and16_ri(REG4,0xfff0);

    VoC_sw16_d(REG0,Output_reserve0_addr);//whether the width has changed;
    VoC_sw16_d(REG4,Input_JPEGENC_WIDTH_addr);
LABLE_WIDTH_MULTIPLE16_YUV:
    VoC_bez16_r(LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV,REG7);
    VoC_add16_rd(REG5,REG5,CONST_15_ADDR);
    VoC_and16_ri(REG5,0xfff0);
    VoC_NOP();
    VoC_sw16_d(REG5,Input_JPEGENC_HEIGHT_addr);
LABLE_WIDTH_HEIGHT_MULTIPLE16_YUV:

    VoC_lw16i_short(REG1,0,DEFAULT);

    ///////////////////////////////////////////////////
LABLE_REG2JPEG_LOOP_HEIGHT_411_YUV:
    VoC_lw16i_short(REG0,-4,DEFAULT);//16=2**4
    VoC_jal(DEMAI_READ_RGBDATA);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,1);
    VoC_lw16i_short(REG5,0,DEFAULT);
LABLE_REG2JPEG_LOOP_WIDTH_411_YUV:
    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);//r0: rgb addr   r1:height loop
    VoC_push16(REG5,IN_PARALLEL);//width loop
    VoC_jal(Convert_YUV411_Block);


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_pop16(REG5,IN_PARALLEL);
    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);
    /////////////////////////////////
    VoC_bnlt16_rd(LABLE_CON1_411_YUV,REG5,Input_JPEGENC_WIDTH_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_WIDTH_411_YUV);
LABLE_CON1_411_YUV:
    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_bnlt16_rd(LABLE_CON2_411_YUV,REG1,Input_JPEGENC_HEIGHT_addr);
    VoC_jump(LABLE_REG2JPEG_LOOP_HEIGHT_411_YUV);
LABLE_CON2_411_YUV:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);
    //whether there is  odd byte left
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

///////////////////1280*1024  yuv422/////////////////////////////////////////////////////
void COOLSAND_Double2Single_YUV(void)
{
    //r1:pin    r0:pout

    //y1 = ((*pin)&0xff);//0
    //u  = (((*pin++)>>8)&0xff);
    //y2 = ((*pin)&0xff);//1
    //v  = (((*pin++)>>8)&0xff);
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,8,DEFAULT);//y1
    VoC_rbinc_i(REG5,8,DEFAULT);//u

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,8,DEFAULT);//y2
    VoC_rbinc_i(REG7,8,DEFAULT);//v
    //wid = wid>>1;
    VoC_loop_i_short(4,DEFAULT);//for (i=wid;i>0;i--)
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;
    VoC_movreg16(REG3,REG5,IN_PARALLEL);//save u

    VoC_rbinc_i(REG4,8,DEFAULT);//y1 = ((*pin)&0xff);//2
    VoC_rbinc_i(REG5,8,DEFAULT);//u1 = (((*pin++)>>8)&0xff);
    VoC_sw16inc_p(REG6,REG0,IN_PARALLEL);//*pout++ = y2;


    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_rbinc_i(REG4,8,DEFAULT);//y22 = ((*pin++)&0xff);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);//y1 u1

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//(u+u1)
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);//y1

    VoC_shru16_ri(REG3,1,DEFAULT);//(u+u1)>>1
    VoC_add16_rr(REG2,REG5,REG6,IN_PARALLEL);//(y1+y2)

    VoC_rbinc_i(REG5,8,DEFAULT);//v1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_sw16inc_p(REG3,REG0,DEFAULT);//*pout++ = (u+u1)>>1;
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);//(v+v1)

    VoC_shru16_ri(REG7,1,DEFAULT);//(v+v1)>>1
    VoC_sw16inc_p(REG2,REG0,IN_PARALLEL);//*pout++ =(y1+y2)>>1;

    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = (v+v1)>>1;


    VoC_endloop0

    VoC_return;
}

void COOLSAND_Double2SingleLastRow_YUV(void)
{
    VoC_lw32z(RL6,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,IN_PARALLEL);

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG4,8,DEFAULT);//y1
    VoC_rbinc_i(REG5,8,DEFAULT);//u

    VoC_lbinc_p(REG1);
    VoC_rbinc_i(REG6,8,DEFAULT);//y2
    VoC_rbinc_i(REG7,8,DEFAULT);//v
    //wid = wid>>1;
    VoC_loop_i_short(3,DEFAULT);//for (i=wid;i>0;i--)
    VoC_startloop0
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;
    VoC_movreg16(REG3,REG5,IN_PARALLEL);//save u

    VoC_rbinc_i(REG4,8,DEFAULT);//y1 = ((*pin)&0xff);//2
    VoC_rbinc_i(REG5,8,DEFAULT);//u1 = (((*pin++)>>8)&0xff);
    VoC_sw16inc_p(REG6,REG0,IN_PARALLEL);//*pout++ = y2;


    VoC_lbinc_p(REG1);//get next 16-bit data
    VoC_rbinc_i(REG4,8,DEFAULT);//y22 = ((*pin++)&0xff);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);//y1 u1

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);//(u+u1)
    VoC_movreg16(REG5,ACC1_LO,IN_PARALLEL);//y1

    VoC_shru16_ri(REG3,1,DEFAULT);//(u+u1)>>1
    VoC_add16_rr(REG2,REG5,REG6,IN_PARALLEL);//(y1+y2)

    VoC_rbinc_i(REG5,8,DEFAULT);//v1
    VoC_shru16_ri(REG2,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_sw16inc_p(REG3,REG0,DEFAULT);//*pout++ = (u+u1)>>1;
    VoC_add16_rr(REG7,REG5,REG7,IN_PARALLEL);//(v+v1)

    VoC_shru16_ri(REG7,1,DEFAULT);//(v+v1)>>1
    VoC_sw16inc_p(REG2,REG0,IN_PARALLEL);//*pout++ =(y1+y2)>>1;

    VoC_movreg32(REG67,REG45,DEFAULT);
    VoC_movreg32(REG45,ACC1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = (v+v1)>>1;

    VoC_endloop0

    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ = y1;
    VoC_add16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1+y2
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_shru16_ri(REG4,1,IN_PARALLEL);//(y1+y2)>>1

    VoC_NOP();
    VoC_sw16inc_p(REG4,REG0,DEFAULT);//*pout++ =(y1+y2)>>1;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = v;

    VoC_sw16inc_p(REG6,REG0,DEFAULT);//*pout++ = y2;
    VoC_sw16inc_p(REG5,REG0,DEFAULT);//*pout++ = u;
    VoC_sw16inc_p(REG6,REG0,DEFAULT);//*pout++ = y2;
    VoC_sw16inc_p(REG7,REG0,DEFAULT);//*pout++ = V;

    VoC_return;
}
void COOLSAND_Double2Row_YUV(void)
{
    //(R2 + R3)/2 -> R0
    VoC_lw32inc_p(REG45,REG2,DEFAULT);

    VoC_loop_i_short(16,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);
    VoC_startloop0

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG7,IN_PARALLEL);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_lw32inc_p(REG67,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG5,1,DEFAULT);

    VoC_lw32inc_p(REG45,REG2,DEFAULT);
    VoC_sw32inc_p(REG45,REG0,DEFAULT);


    VoC_endloop0

    VoC_return;
}

void COOLSAN_Encode422MCUBlock_YUV(void)
{
    //r0:rgb buf(Y)  r1:Y(X)   r2:Y1(X)   r3:u v(X)
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i(REG5,128);



    VoC_loop_i(1,8);

    VoC_loop_i(0,4);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//y11 = (*ptemp++)-128;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//u =  (*ptemp++)-128;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG7,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG1,DEFAULT);//*y1_ptr++ = y11;
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//y22 = (*ptemp++)-128;

    VoC_sw16_p(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);//*cb_ptr++ = u;

    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//v =  (*ptemp++)-128;
    VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);//v addr

    VoC_sw16inc_p(REG6,REG1,DEFAULT);//*y1_ptr++ = y22;
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG4,DEFAULT);//*cr_ptr++ = v;

    VoC_endloop0
    ///////////////////////////////////////////////////////

    VoC_loop_i(0,4);

    VoC_lw32inc_p(REG67,REG0,DEFAULT);

    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//y11 = (*ptemp++)-128;
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//u =  (*ptemp++)-128;

    VoC_lw32inc_p(REG67,REG0,DEFAULT);
    VoC_movreg16(ACC0_LO,REG7,IN_PARALLEL);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);//*y1_ptr++ = y11;
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//y22 = (*ptemp++)-128;

    VoC_sw16_p(ACC0_LO,REG3,DEFAULT);
    VoC_lw16i_short(REG4,64,IN_PARALLEL);//*cb_ptr++ = u;

    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);//v =  (*ptemp++)-128;
    VoC_add16_rr(REG4,REG4,REG3,IN_PARALLEL);//v addr

    VoC_sw16inc_p(REG6,REG2,DEFAULT);//*y1_ptr++ = y22;
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_sw16inc_p(REG7,REG4,DEFAULT);//*cr_ptr++ = v;
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1


    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;


}
void COOLSAND_MEMCopy_YUV(void)
{
    //r2:in   r3:out
    //used  buf: r2 r3 RL6 rl7
    VoC_loop_i_short(8,DEFAULT)
    VoC_startloop0
    VoC_lw32inc_p(RL6,REG2,DEFAULT);
    VoC_lw32inc_p(RL7,REG2,DEFAULT);
    VoC_sw32inc_p(RL6,REG3,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_endloop0

    VoC_return;

}
void COOLSAND_1280BY1024_YUV422_JPEG(void)
{
//#if 0
//  voc_short TEMP_BUFF_FOR1280BY1024_TMP1[48]  ,  y
//  voc_short TEMP_BUFF_FOR1280BY1024_TMP2[48]  ,  y
//#endif

    VoC_lw16i_short(REG5,7,DEFAULT);//line2
    VoC_lw16i_short(REG4,0,IN_PARALLEL);//line1

    VoC_push16(RA,DEFAULT);
LABLE_START_1280BY1024_YUV:
    VoC_push32(REG45,DEFAULT);//line2  line1
    //even-16   first-8
    VoC_lw16i_short(REG0,79,IN_PARALLEL);//count=79;

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_ADDR,1);//pin_addr
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Even);
LABLE_DO_BEGIN_FRONT_EVEN_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r0:pout
    VoC_jal(COOLSAND_Double2Single_YUV);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//int
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);      //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2Single_YUV);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);


    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW1_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_EVEN_YUV);
LABLE_LASTROW1_YUV:
    //last row
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);//r2:pout
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//0


    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);              //1

    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//3


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//5

    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);//7

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//4
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);
    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_3_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;

LABLE_DO_BEGIN_BEHIND_EVEN_YUV:
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    //奇数行
    VoC_jal(COOLSAND_Double2Single_YUV);


    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW2_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_EVEN_YUV);
LABLE_LASTROW2_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    //奇数行
    //double2single(8,pin+640*3,bufftmp);//7
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640*4,pout+48);//9
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_1_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*5,pout+48*3);//11
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_3_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*6,pout+48*5);//13
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_5_ADDR);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*7,pout+48*7);//15
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i(REG0,GLOBAL_OUT_32_7_ADDR);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);
    //偶数行
    //  double2row(16,bufftmp,pout+48,pout);//8
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48,pout+48*3,pout+48*2);//10
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_1_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_3_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*3,pout+48*5,pout+48*4);//12
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_3_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_5_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*5,pout+48*7,pout+48*6);//14
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_5_ADDR,2);//2
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_7_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    //ODD-16 FIRST-8
    VoC_lw32_sd(REG45,0,DEFAULT);//LINE2 LINE1
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);//      line1+=7;
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);//  line2+=7;
    VoC_jal(COOLSAND_DMAIRead_1280BY1024Odd);
    VoC_sw32_sd(REG45,0,DEFAULT);//save line2 line1
    VoC_lw16i_short(REG0,79,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_ADDR,1);
LABLE_DO_BEGIN_FRONT_ODD_YUV:
    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single_YUV);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW3_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_FRONT_ODD_YUV);
LABLE_LASTROW3_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin,bufftmp);   //-1
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640,pout+48*2);  //2
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*2,pout+48*4);//4
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);


    //  double2single(8,pin+640*3,pout+48*6);//6
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*4,&bufftmp[48]);//8
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP2,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2row(16,bufftmp,pout+48*2,pout);//0
    VoC_lw16i_set_inc(REG2,TEMP_BUFF_FOR1280BY1024_TMP1,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);


    // memcpy(pout+48,pout,48*2);            //1
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//in
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_1_ADDR,2);//out
    VoC_jal(COOLSAND_MEMCopy_YUV);

    //奇数行
    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//3
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//5
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,&bufftmp[48],pout+48*7);//7
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP2,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    ////evev-16  second-8////////////////////
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_RGB_640_4_ADDR,1);
    VoC_lw16i(REG0,79);//count=79;
LABLE_DO_BEGIN_BEHIND_ODD_YUV:
    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    exit_on_warnings = OFF;
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count
    VoC_push32(REG01,IN_PARALLEL);
    exit_on_warnings = ON;
    VoC_jal(COOLSAND_Double2Single_YUV);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2Single_YUV);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2Single_YUV);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG01,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_8_ADDR);//updat pin_addr
    VoC_sub16_rd(REG0,REG0,CONST_1_ADDR);//update count
    VoC_bngtz16_r(LABLE_LASTROW4_YUV,REG0);
    VoC_jump(LABLE_DO_BEGIN_BEHIND_ODD_YUV);
LABLE_LASTROW4_YUV:
    VoC_push16(REG1,DEFAULT);//r1:pin    r0:count

    //  double2single(8,pin+640*4,pout);//8
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_FOR1280BY1024_ADDR,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //double2single(8,pin+640*5,pout+48*2);//10
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_2_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*6,pout+48*4);//12
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_4_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*7,pout+48*6);//14
    VoC_lw16_sd(REG1,0,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_6_ADDR,1);
    VoC_sw16_sd(REG1,0,DEFAULT);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);

    //  double2single(8,pin+640*8,bufftmp);//-2
    VoC_pop16(REG1,DEFAULT);
    VoC_add16_rd(REG1,REG1,CONST_640_ADDR);
    VoC_lw16i_set_inc(REG0,TEMP_BUFF_FOR1280BY1024_TMP1,1);
    VoC_jal(COOLSAND_Double2SingleLastRow_YUV);


    //奇数行
    //  double2row(16,pout,pout+48*2,pout+48*1);//9
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_FOR1280BY1024_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_2_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_1_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*2,pout+48*4,pout+48*3);//11
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_2_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_4_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_3_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*4,pout+48*6,pout+48*5);//13
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_4_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,GLOBAL_OUT_32_6_ADDR,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_5_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  double2row(16,pout+48*6,bufftmp,pout+48*7);//15
    VoC_lw16i_set_inc(REG2,GLOBAL_OUT_32_6_ADDR,2);//6
    VoC_lw16i_set_inc(REG3,TEMP_BUFF_FOR1280BY1024_TMP1,2);
    VoC_lw16i_set_inc(REG0,GLOBAL_OUT_32_7_ADDR,2);
    VoC_jal(COOLSAND_Double2Row_YUV);

    //  encode_blocMUC422(pout);
    VoC_jal(COOLSAN_Encode422MCUBlock_YUV);

    VoC_pop32(REG45,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);
    //update line1 and line2
    VoC_add16_rr(REG4,REG4,REG6,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG6,IN_PARALLEL);

    VoC_bgt16_rd(LABLE_END_1280BY1024_YUV,REG5,CONST_479_ADDR);
    VoC_jump(LABLE_START_1280BY1024_YUV);
LABLE_END_1280BY1024_YUV:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;


}
////    VoC_NOP();
////    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
//
//  VoC_return;
//}

void Coolsand_zoomline_yuv(void)
{
    //R0: IN BUF  inc0=2  inc1=1
    //R2: OUT BUF
    //used buf: all
    VoC_lw16i(REG3,Q15);

    VoC_lw32z(REG67,DEFAULT);//wn = 0;
    VoC_lw32z(RL7,IN_PARALLEL);

    VoC_add16_rd(REG1,REG2,Input_JPEGENC_WIDTH_addr);//u v addr
    /////////////loop1/////////////////////////////////////////
    VoC_lw16i_short(REG4,1,DEFAULT);//VoC_lw32z(REG45,DEFAULT);//w
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_lw16_d(REG5,Input_JPEGENC_WIDTH_addr);
    VoC_shru16_ri(REG5,1,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);



    VoC_loop_r(0,REG5);//for (i=0;i<dec_w/2;i++)

    VoC_shru16_ri(REG4,-1,DEFAULT);//w<<1
    VoC_movreg16(REG5,REG4,IN_PARALLEL);//w
    /////////u v//////////
    VoC_movreg16(REG1,REG0,DEFAULT);//srcbuf
    VoC_movreg32(RL6,REG01,IN_PARALLEL);

    VoC_add32_rr(REG01,REG01,REG45,DEFAULT);//pa = srcbuf + (w<<1); pa = srcbuf + w ;
    VoC_sub16_rr(REG7,REG3,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_add32_rd(RL7,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;

    VoC_lw32inc_p(REG45,REG0,DEFAULT);//u = (*(pa))>>8;    v = (*(pa+1))>>8;
    VoC_lw16i_short(INC1,1,IN_PARALLEL);

    VoC_shru16_ri(REG4,8,DEFAULT);//u
    VoC_shru16_ri(REG5,8,IN_PARALLEL);//v

    VoC_multf32_rr(ACC0,REG5,REG7,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);

    VoC_lw32_p(REG45,REG0,DEFAULT);//u1 = (*(pa+2))&0xff;  v1 = (*(pa+3))&0xff;
    VoC_lw16i_short(INC2,1,IN_PARALLEL);

    VoC_shru16_ri(REG4,8,DEFAULT);
    VoC_shru16_ri(REG5,8,IN_PARALLEL);

    VoC_mac32_rr(REG5,REG6,DEFAULT);//(v*wa+v1*wb)
    VoC_mac32_rr(REG4,REG6,IN_PARALLEL);//(u*wa+u1*wb)

    VoC_lw16inc_p(REG4,REG1,DEFAULT);//y

    VoC_lw16_p(REG5,REG1,DEFAULT);//y1
    VoC_movreg32(REG01,RL6,IN_PARALLEL);

    VoC_and32_rd(REG45,CONST_0XFF00FF_ADDR);

    VoC_sw16inc_p(ACC1_HI,REG1,DEFAULT);//*pbu++ = (u*wa+u1*wb)>>S15;
    VoC_multf32_rr(ACC0,REG6,REG5,IN_PARALLEL);

    VoC_mac32_rr(REG7,REG4,DEFAULT);//(y*wa + y1*wb)
    VoC_sw16inc_p(ACC0_HI,REG1,IN_PARALLEL);//*pbu++ = (v*wa+v1*wb)>>S15;

    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_movreg32(REG45,RL7,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_shru32_ri(REG45,S15,IN_PARALLEL);//w  = (wn>>S15);

    VoC_endloop0
    ////////////////loop2//////////////////////////////////////////////////////////
    VoC_sub16_rd(REG5,REG1,CONST_2_ADDR);
    VoC_lw32_d(ACC1,GLOBAL_MASK_ADDR);
    VoC_lw32_p(ACC0,REG5,DEFAULT);
    VoC_lw16_d(REG5,Input_JPEGENC_WIDTH_addr);
    VoC_add16_rr(REG1,REG0,REG4,DEFAULT);//srcbuf + w
    VoC_sw32_p(ACC0,REG1,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);

    VoC_loop_r(0,REG5);//for (i=0;i<dec_w;i++)
    //r6;wb   r4:w

    VoC_add32_rd(RL7,RL7,GLOBAL_SCALEX_SIZE_ADDR);//wn+=scalex;

    VoC_lw16inc_p(REG5,REG1,DEFAULT);//y1
    VoC_sub16_rr(REG7,REG3,REG6,IN_PARALLEL);//wa = Q15 - wb;

    VoC_lw16_p(REG4,REG1,DEFAULT);//y
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_and32_rd(REG45,CONST_0XFF00FF_ADDR);

    //VoC_multf32_rr(ACC0,REG4,REG7,DEFAULT);
    //VoC_mac32_rr(REG5,REG6,DEFAULT);
    VoC_bimac32_rr(REG45,REG67);

    VoC_movreg32(REG45,RL7,DEFAULT);//wn
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_and32_rr(REG67,ACC1,DEFAULT);//wb = wn&MASK;
    VoC_shru32_ri(REG45,S15,IN_PARALLEL);//w  = (wn>>S15);

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;
    VoC_add16_rr(REG1,REG0,REG4,IN_PARALLEL);//srcbuf + w

    VoC_endloop0

    VoC_sw16inc_p(ACC0_HI,REG2,DEFAULT);//*pb++ =(y*wa + y1*wb)>>S15;

    VoC_return;
}

/************************************************************************/
/* y...uv.....  -> y-128...... u-128 v-128                              */
/************************************************************************/
void  COOLSAND_AverageTwoLine(void)
{
    //outbuf: r1
    //inbuf:  r2 r3   r6:wb  r7:wa

    VoC_lw16i_short(INC1,2,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);
    VoC_xor16_rd(REG4,GLOBAL_FlagForAverage_ADDR);
    VoC_lw16i(REG0,128);
    VoC_sw16_d(REG4,GLOBAL_FlagForAverage_ADDR);

    //y
    VoC_lw16_d(REG4,Input_JPEGENC_WIDTH_addr);
    VoC_shru16_ri(REG4,2,DEFAULT);
    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);
    VoC_loop_r(0,REG4);//for (i=0;i<dec_w;i++)
    //r6;wb   r4:w
    //VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    //VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);

    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG0,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG0,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG0,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG0,DEFAULT);

    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);

    VoC_endloop0

    //uv//////////////////////////////////////////////////////////////
    VoC_bnez16_d(LABLE_COMPUTE_UV,GLOBAL_FlagForAverage_ADDR);
    VoC_return;
LABLE_COMPUTE_UV:
    VoC_lw16_d(REG4,Input_JPEGENC_WIDTH_addr);
    VoC_shru16_ri(REG4,2,DEFAULT);

    VoC_loop_r(0,REG4);//for (i=0;i<dec_w;i++)

    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_mac32inc_rp(REG7,REG2,DEFAULT);
    VoC_mac32inc_rp(REG7,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG4,REG0,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG0,IN_PARALLEL);

    VoC_movreg16(REG4,ACC0_HI,DEFAULT);
    VoC_movreg16(REG5,ACC1_HI,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG0,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG0,DEFAULT);

    VoC_multf32inc_rp(ACC0,REG6,REG3,DEFAULT);
    VoC_multf32inc_rp(ACC1,REG6,REG3,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG1,DEFAULT);

    VoC_endloop0

    VoC_return;


}
void CoolSand_SignBits2(void)
{
    //reg4: x;  reg5: n
    //used buf: reg3
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_lw16i_short(REG5,-30,IN_PARALLEL);
Lable_X_L1:
    //for( n = 30 x >>= 1; --n );
    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG3,IN_PARALLEL);
    VoC_bnez16_r(Lable_X_L1,REG4);

    VoC_return;//return n;
}


void CoolSand_Div2(void)
{
    //reg6:     divd;  reg7: divs
    //output:   rl6_lo
    //used buf:r3 r4 r5  r6 r7 acc0 acc1 rl6
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);
    //dexp = signbits(divd)-1;
    VoC_movreg16(REG4,REG6,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);
    VoC_jal(CoolSand_SignBits2);
    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_movreg16(ACC0_LO,REG6,IN_PARALLEL);
    //sexp = signbits(divs)-1;
    VoC_movreg16(REG4,REG7,DEFAULT);
    VoC_lw32z(ACC1,IN_PARALLEL);
    VoC_jal(CoolSand_SignBits2);
    VoC_movreg16(ACC1_LO,REG7,DEFAULT);
    VoC_shru32_rr(ACC0,REG6,IN_PARALLEL);//acc0:divd = divd << (dexp);

    VoC_shru32_rr(ACC1,REG5,DEFAULT);//acc1:divs = divs << (sexp);
    VoC_sub16_rr(REG4,REG5,REG6,IN_PARALLEL);

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);//r4: exp  = dexp-sexp+4;
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//r67: 0x0001

    VoC_lw16i_short(REG3,S15+4,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
LABLE_DIV_COUNTDOWN:
    VoC_bgt32_rr(LABLE_DIV_L0,ACC1,ACC0);//if (divs <= divd)
    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);//divd = divd - divs;
    VoC_add32_rr(RL6,RL6,REG67,IN_PARALLEL);//r3 = r3 + r2;
LABLE_DIV_L0:
    VoC_shru32_ri(RL6,-1,DEFAULT);//r3   <<= 1;
    VoC_shru32_ri(ACC0,-1,IN_PARALLEL);//divd <<= 1;

    VoC_inc_p(REG3,DEFAULT);
    VoC_bnez16_r(LABLE_DIV_COUNTDOWN,REG3);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_shru32_rr(RL6,REG4,DEFAULT);//r3=r3>>exp;    //loop - exp

    VoC_return;//return (int)r3;
}

/************************************************************************/
/*
  y --------------------
  u v-------------------
  y---------------------                                                                     */
/************************************************************************/
void COOLSAND_ENCODE_YUVBLOCK(void)
{

    VoC_lw16_d(REG7,Input_JPEGENC_WIDTH_addr);
    VoC_sub16_rd(REG4,REG7,CONST_16_ADDR);//temp=width-16;

    VoC_lw16_d(REG1,Input_JPEGENC_WIDTH_addr);
    VoC_shru16_ri(REG1,4,DEFAULT);//VoC_lw16i_short(REG1,11,DEFAULT);
    VoC_add16_rr(REG5,REG7,REG4,IN_PARALLEL);//temp+width;

    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,2);

    VoC_push16(REG7,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);


LABLE_LOOP_LINEBY16:   //VoC_loop_i(1,11)//176/16=11

    VoC_push32(REG01,DEFAULT);//pyuv = (short*)buff;
    VoC_push16(RA,IN_PARALLEL);

    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y_ADDR,2);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_Y1_ADDR,2);
    //y1 and y2   //y3 and y4
    VoC_loop_i(1,2)

    VoC_loop_i(0,4)//for (i=0;i<4;i++)

    //for (j=0;j<8;j++) *py1++ = *pyuv++;
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);


    //for (j=0;j<8;j++) *py2++ = *pyuv++;
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32_sd(REG45,1,IN_PARALLEL);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG5,IN_PARALLEL);//pyuv+=temp+width;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);


    //for (j=0;j<8;j++)//*py1++ = *pyuv++;
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);

    //for (j=0;j<8;j++)//*py2++ = *pyuv++;
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG4,IN_PARALLEL);//pyuv+=temp;
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_endloop0
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y2_ADDR,2);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_Y3_ADDR,2);
    VoC_endloop1


    //u and v
    //pyuv = (short*)buff+width;
    //VoC_lw16_d(REG7,Input_JPEGENC_WIDTH_addr);
    VoC_lw32_sd(REG01,0,DEFAULT);
    VoC_lw16_sd(REG7,1,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG7,DEFAULT);
    VoC_add16_rr(REG6,REG5,REG7,IN_PARALLEL);//temp+width*2;
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_CB_ADDR,2);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CR_ADDR,2);

    VoC_loop_i(0,8)//for (i=0;i<8;i++)

    //for (j=0;j<8;j++)
    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);//*pcb++ = *pyuv++;//*pcr++ = *pyuv++;

    VoC_movreg16(ACC0_HI,ACC1_LO,DEFAULT);
    VoC_movreg16(ACC1_LO,ACC0_HI,IN_PARALLEL);

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG0,DEFAULT);

    VoC_movreg16(RL6_HI,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,RL6_HI,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);

    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_sw32inc_p(RL7,REG3,DEFAULT);

    VoC_lw32inc_p(ACC0,REG0,DEFAULT);
    VoC_lw32inc_p(ACC1,REG0,DEFAULT);

    VoC_movreg16(ACC0_HI,ACC1_LO,DEFAULT);
    VoC_movreg16(ACC1_LO,ACC0_HI,IN_PARALLEL);

    VoC_lw32inc_p(RL6,REG0,DEFAULT);
    VoC_lw32inc_p(RL7,REG0,DEFAULT);

    VoC_movreg16(RL6_HI,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,RL6_HI,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);

    VoC_sw32inc_p(RL6,REG2,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG6,IN_PARALLEL);//pyuv+=temp+width*2;
    VoC_sw32inc_p(RL7,REG3,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);
    VoC_endloop0

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y2_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y3_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr); r7=1

    //update
    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(REG6,16,IN_PARALLEL);
    VoC_add16_rr(REG0,REG0,REG6,DEFAULT);
    VoC_sub16_rr(REG1,REG1,REG7,IN_PARALLEL);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(INC0,2,IN_PARALLEL);

    VoC_bez16_r(LABLE_QUITE_ENCODE_IMCU,REG1);
    VoC_jump(LABLE_LOOP_LINEBY16);

LABLE_QUITE_ENCODE_IMCU:
    VoC_pop16(REG7,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_return;
}

void  COOLSAND_ZOOM_JpgeEnc(void)
{
    VoC_push16(RA,DEFAULT);
//  VoC_lw16i_short(REG7,15,IN_PARALLEL);
//  //make height is the multple of 16
//  VoC_add16_rd(REG6,REG7,Input_JPEGENC_HEIGHT_addr);
//  VoC_shru16_ri(REG6,4,DEFAULT);
//  VoC_shru16_ri(REG6,-4,DEFAULT);
//  VoC_sw16_d(REG6,Input_JPEGENC_HEIGHT_addr);
    //compute scalex and scaley
    VoC_lw32_d(REG67,Input_samplerate_addr);//sourse resolution
    VoC_lw32_d(RL7,Input_JPEGENC_WIDTH_addr);//object resolution

    VoC_movreg16(REG7,RL7_LO,DEFAULT);
    VoC_movreg16(RL7_LO,REG7,IN_PARALLEL);
    VoC_jal(CoolSand_Div2);//scalex=(cut_w<<S15)/dec_w;
    VoC_movreg32(REG67,RL7,DEFAULT);
    VoC_sw32_d(RL6,GLOBAL_SCALEX_SIZE_ADDR);
    VoC_jal(CoolSand_Div2);//scaley=(cut_h<<S15)/dec_h;
    VoC_sw32_d(RL6,GLOBAL_SCALEY_SIZE_ADDR);

    //Compute DMA size and const values
    VoC_lw16i_short(REG5,INLINE_NUM,DEFAULT);
    VoC_lw16i_short(REG6,80,IN_PARALLEL);

    VoC_mult16_rr(REG5,REG6,REG5,DEFAULT);//DMA SIZE[INT]=NUM*160/2
    VoC_lw16i_short(FORMAT32,-2,IN_PARALLEL);

    VoC_lw16i(ACC1_LO,0XFF);
    VoC_lw16i_short(REG4,INLINE_NUM_1,DEFAULT);
    VoC_movreg16(ACC1_HI,ACC1_LO,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG4,REG6,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);
    VoC_sw32_d(REG45,GLOBAL_ORGI_NUM_1_ADDR);
    VoC_lw16i(RL6_LO,MASK);
    VoC_sw32_d(ACC0,GLOBAL_ORGI_DMAINC_ADDR);
    VoC_lw16i_short(REG6,-2,DEFAULT);//old num1 (num2)
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//num3
    VoC_sw32_d(ACC1,CONST_0XFF00FF_ADDR);
    VoC_sw32_d(REG67,GLOBAL_NUM1_ADDR);
    VoC_sw16_d(REG7,GLOBAL_FlagForAverage_ADDR);
    VoC_lw16i(REG6,GLOBAL_ORGI_SOURCE_ADDR);
    //VoC_lw16i(REG7,GLOBAL_ORG_END_ADDR);
    VoC_lw16i_short(REG5,24,DEFAULT);
    VoC_mult16_rd(REG5,REG5,Input_JPEGENC_WIDTH_addr);
    VoC_lw16i(REG7,JPEGENC_BUFF_RGB_ADDR);
    VoC_add16_rr(REG7,REG5,REG7,DEFAULT);
    VoC_sw32_d(RL6,GLOBAL_MASK_ADDR);
    VoC_sw32_d(REG67,GLOBAL_INPUT_ADDR_ADDR);
    //main entrance
    //VoC_jal(COOLSAND_DMA_READ_ORG);
    VoC_lw16i(REG5,GLOBAL_ORGI_SOURCE_ADDR/2);//VoC_shr16_ri(REG0,1,IN_PARALLEL);//dmai-addr[int]
    VoC_lw16_d(REG4,GLOBAL_ORGI_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,Input_inStreamBufAddr_addr);//[BYTE]
    ////////dmai//////////////////
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_ORGI_DMAINC_ADDR);
    VoC_NOP();
    VoC_sw32_d(ACC0,Input_inStreamBufAddr_addr);

    VoC_lw16_d(REG4,Input_JPEGENC_HEIGHT_addr);//VoC_lw16i(REG4,144);//dec_h
    VoC_lw32z(REG67,DEFAULT);//w
//  VoC_lw16i_short(REG4,15,IN_PARALLEL);
//  VoC_add16_rr(REG4,REG4,Input_JPEGENC_HEIGHT_addr);
//  VoC_shru16_ri(REG4,4,DEFAULT);
//  VoC_shru16_ri(REG4,-4,IN_PARALLEL);

    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);
    VoC_lw16i(REG1,JPEGENC_BUFF_RGB_ADDR);
    VoC_push16(REG7,DEFAULT);//extera

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

LABLE_LOOP_ZOOM:
    VoC_lw16_sd(REG5,0,DEFAULT);//extra
    VoC_push16(REG4,DEFAULT);//DEC_h
    VoC_push32(REG67,IN_PARALLEL);//W

    VoC_shru32_ri(REG67,S15,DEFAULT);//r6 = num1 = (w>>S15);
    VoC_push16(REG1,IN_PARALLEL);
    VoC_sub16_rr(REG6,REG6,REG5,DEFAULT);//num1 = (w>>S15) - extra;
    VoC_lw16_d(REG7,GLOBAL_NUM1_ADDR);//r7 = old num1
    //need to refill buf of input
    VoC_blt16_rd(LABLE_PREVIEW_L0,REG6,GLOBAL_ORGI_NUM_1_ADDR);//if (num1>=numline-1)
    //VoC_jal(COOLSAND_DMA_READ_ORG);//memcpy(bufin,pin,numcount);//pin+=(numline-1)*src_w;
    VoC_lw16i(REG5,GLOBAL_ORGI_SOURCE_ADDR/2);//VoC_shr16_ri(REG0,1,IN_PARALLEL);//dmai-addr[int]
    VoC_lw16_d(REG4,GLOBAL_ORGI_DMASIZE_ADDR);
    VoC_lw32_d(ACC0,Input_inStreamBufAddr_addr);//[BYTE]
    ////////dmai//////////////////
    VoC_sw32_d(REG45,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_ORGI_DMAINC_ADDR);
    VoC_lw16_d(REG5,GLOBAL_ORGI_NUM_1_ADDR);//VoC_NOP();
    VoC_sw32_d(ACC0,Input_inStreamBufAddr_addr);

    VoC_lw16_sd(REG4,2,DEFAULT);//extra
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);//num1-=numline-1;

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG5,DEFAULT);
    VoC_sw16_sd(REG4,2,DEFAULT);//save extra
    //VoC_lw16i_short(REG5,0,DEFAULT);
    //VoC_shru32_ri(REG45,-S15,DEFAULT);//w = num1<<S15;
    //VoC_sw32_sd(REG45,0,DEFAULT);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
LABLE_PREVIEW_L0:

    //zoom line
    VoC_lw16i(REG5,160);
    VoC_mult16_rr(REG4,REG6,REG5,DEFAULT);         //p3 = pa + (num1*src_w);
    VoC_sub16_rr(REG7,REG6,REG7,IN_PARALLEL);      //r7=num1-num2
    VoC_sw16_d(REG6,GLOBAL_NUM1_ADDR);
    VoC_add16_rd(REG4,REG4,GLOBAL_INPUT_ADDR_ADDR);//r4=P3
    VoC_add16_rr(REG5,REG4,REG5,DEFAULT);          //r5=p4 = p3 + src_w;
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    //switch
    VoC_bne16_rr(LABLE_PREVIEW_L1,REG7,REG6)//if (num1-num2==1)

    VoC_bnez16_d(LABLE_PREVIEW_L3,GLOBAL_NUM3_ADDR);//if (num3==0)
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF_ADDR,2);//ptmp2    p2 = tmpbuf;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF2_ADDR,1);//tmpbuf   p1 = ptmp2;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3=1;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,tmpbuf,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L3://else
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_movreg16(REG2,REG3,DEFAULT);//ptmp2
    VoC_movreg16(REG0,REG5,IN_PARALLEL);//P4
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
    VoC_jump(LABLE_PREVIEW_L2);
LABLE_PREVIEW_L1:
    VoC_bez16_r(LABLE_PREVIEW_L2,REG7);//else if (num1-num2!=0)
    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_set_inc(REG3,GLOBAL_TEMP_BUF2_ADDR,2);//ptmp2  p2 = ptmp2;
    VoC_lw16i_set_inc(REG2,GLOBAL_TEMP_BUF_ADDR,1);//tmpbuf   p1 = tmpbuf;
    VoC_sw16_d(REG6,GLOBAL_NUM3_ADDR);//num3 = 0;
    VoC_movreg16(REG0,REG4,DEFAULT);
    VoC_push16(REG5,IN_PARALLEL);
    VoC_sw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p3,tmpbuf,dec_w,scalex);
    VoC_pop16(REG0,DEFAULT);
    VoC_lw16i(REG2,GLOBAL_TEMP_BUF2_ADDR);
    VoC_jal(Coolsand_zoomline_yuv);//zoom_line(p4,ptmp2,dec_w,scalex);
LABLE_PREVIEW_L2:

    VoC_lw16i_set_inc(REG3,Q15,2);

    VoC_lw32_sd(REG67,0,DEFAULT);//wn
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_and32_rd(REG67,GLOBAL_MASK_ADDR);//wb = wn&MASK;
    VoC_sub16_rr(REG7,REG3,REG6,DEFAULT);//wa = Q15 - wb;
    VoC_pop16(REG1,IN_PARALLEL);
    VoC_lw32_d(REG23,GLOBAL_P1_ADDR);
    VoC_jal(COOLSAND_AverageTwoLine);

    VoC_bne16_rd(LABLE_JUMP2LOOP,REG1,GLOBAL_INPUT_END_ADDR_ADDR);
    //JPEG ENOCDE PART
    VoC_jal(COOLSAND_ENCODE_YUVBLOCK);
    //VoC_jal(TEMP_DMA_WRITE);
    VoC_lw16i(REG1,JPEGENC_BUFF_RGB_ADDR);
    VoC_lw16i_short(FORMATX,S15-16,DEFAULT);
    VoC_lw16i_short(FORMAT32,S15-16,IN_PARALLEL);
LABLE_JUMP2LOOP:
    VoC_pop16(REG4,DEFAULT);//DEC_h
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_sub16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);
    VoC_add32_rd(REG67,REG67,GLOBAL_SCALEY_SIZE_ADDR);//update

    VoC_bez16_r(LABLE_QUITE,REG4);
    VoC_jump(LABLE_LOOP_ZOOM);
LABLE_QUITE:

    VoC_lw16i_short(REG4,15,DEFAULT);
    VoC_and16_rd(REG4,Input_JPEGENC_HEIGHT_addr);
    VoC_bez16_r(LABLE_ZOOM_ENC_HEIGHT_LOOP_END,REG4);
    VoC_jal(COOLSAND_ENCODE_YUVBLOCK);
LABLE_ZOOM_ENC_HEIGHT_LOOP_END:
    //end
    VoC_jal(COOLSNAD_flush_bits);
    VoC_pop16(REG3,DEFAULT);//extra
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

//void  TEMP_DMA_WRITE(void)
//{
//
//      VoC_lw16i(REG3,JPEGENC_BUFF_RGB_ADDR/2);
//    VoC_lw16i(REG2,2112);

/**************************************************************************************
 * Function:    COOLSAND_JPEGENCODE_WITHOUT_ZOOM
 *
 * Description: no
 *
 * Inputs:  REG0
 *          REG1
 *          REG2
 *
 * Outputs: REG0
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/14/2012
 **************************************************************************************/
void COOLSAND_JpegEncodeWithoutZoom(void)
{

    VoC_lw32z(REG01,DEFAULT);

    VoC_push16(RA,DEFAULT);

COOLSAND_JpegEncodeWithoutZoom_L0:

    VoC_push32(REG01,DEFAULT);//r0: width loop   r1:height loop

    VoC_jal(COOLSAND_JpegDmaMcu);

    VoC_jal(COOLSAND_JpegBlock);

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(REG4,16,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    /////////////////////////////////
    VoC_bnlt16_rd(COOLSAND_JpegEncodeWithoutZoom_L1,REG0,Input_JPEGENC_WIDTH_addr);
    VoC_jump(COOLSAND_JpegEncodeWithoutZoom_L0);
COOLSAND_JpegEncodeWithoutZoom_L1:

    VoC_lw16i_short(REG3,14,DEFAULT);
    VoC_lw16i_short(REG4,8,IN_PARALLEL);

    VoC_mult16_rd(REG2,REG3,Input_JPEGENC_WIDTH_addr);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

    VoC_add32_rd(REG23,REG23,Input_inStreamBufAddr_addr);//NEW EX_ADDR [BYTE]

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop

    VoC_sw32_d(REG23,Input_inStreamBufAddr_addr);

    VoC_bnlt16_rd(COOLSAND_JpegEncodeWithoutZoom_L2,REG1,Input_JPEGENC_HEIGHT_addr);
    VoC_jump(COOLSAND_JpegEncodeWithoutZoom_L0);
COOLSAND_JpegEncodeWithoutZoom_L2:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    COOLSAND_JpegDmaMcu
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/14/2012
 **************************************************************************************/
void COOLSAND_JpegDmaMcu(void)
{

    VoC_lw32_d(ACC0,Input_inStreamBufAddr_addr);//[BYTE]

    VoC_lw16i(REG6,JPEGENC_BUFF_RGB_ADDR/2);//dmai-addr[int]

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG2,8*2*2,IN_PARALLEL);

    VoC_add32_rr(RL6,REG23,ACC0,DEFAULT);//NEW EX_ADDR [BYTE]
    VoC_lw16i_short(REG0,8,IN_PARALLEL);

    VoC_lw16i_short(REG2,16,DEFAULT);

    VoC_sub16_dr(REG3,Input_JPEGENC_WIDTH_addr,REG2);

    VoC_shr16_ri(REG3,1,DEFAULT);

    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_lw16i_short(REG2,0,IN_PARALLEL);

    VoC_sw16_d(REG3,CFG_DMA_2D_STRIDE);     //(width-8*2)/2 int
    VoC_sw16_d(REG0,CFG_DMA_2D_LINES);      //8

    VoC_lw16i(REG3,0x1000);

    VoC_or32_rr(ACC0,REG23,DEFAULT);

    VoC_sw16_d(REG6,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);          //16/2
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_sw32_d(RL6,Input_inStreamBufAddr_addr);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_bnez16_d(COOLSAND_JpegDmaMcu_L0,Input_Channel_Num_addr)

    VoC_mult16_rr(REG1,REG4,REG0,DEFAULT);//lines*DMA_SIZE
    VoC_shr16_ri(REG6,-1,IN_PARALLEL);//

    VoC_movreg16(REG0,REG6,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);


    VoC_jal(COOLSAND_RGBToYUV422);

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

COOLSAND_JpegDmaMcu_L0:

    VoC_return;
}

/**************************************************************************************
 * Function:    COOLSAND_JpegBlock
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/15/2012
 **************************************************************************************/
void COOLSAND_JpegBlock(void)
{
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_RGB_ADDR,2);
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_set_inc(REG3,JPEGENC_BUFF_CR_ADDR,1);

    VoC_lw16i(ACC0_HI,JPEGENC_BUFF_Y1_ADDR);

    VoC_movreg16(ACC0_LO,REG1,DEFAULT);

    VoC_lw16i(REG6,128);

    VoC_loop_i(1,8);

    VoC_loop_i_short(4,DEFAULT);
    VoC_movreg16(REG1,ACC0_LO,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);//y
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//u

    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_movreg16(REG7,REG5,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);//y

    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//v

    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_movreg16(ACC0_LO,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_loop_i_short(4,DEFAULT);
    VoC_movreg16(REG1,ACC0_HI,IN_PARALLEL);
    VoC_startloop0

    VoC_lw32inc_p(REG45,REG0,DEFAULT);

    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);//y1
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//u

    VoC_lw32inc_p(REG45,REG0,DEFAULT);
    VoC_movreg16(REG7,REG5,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG6,IN_PARALLEL);//y1

    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG6,IN_PARALLEL);//v

    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_movreg16(ACC0_HI,REG1,IN_PARALLEL);

    VoC_endloop0

    VoC_NOP();

    VoC_endloop1

    VoC_return;
}

/**************************************************************************************
 * Function:    COOLSAND_JpegEncodeWithZoom
 *
 * Description: no
 *
 * Inputs:  REG0
 *          REG1
 *          REG2
 *
 * Outputs: REG0
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/20/2012
 **************************************************************************************/
void COOLSAND_JpegEncodeWithZoom(void)
{

    VoC_lw32z(REG01,DEFAULT);

    VoC_push16(RA,DEFAULT);

COOLSAND_JpegEncodeWithZoom_L0:

    VoC_push32(REG01,DEFAULT);//r0: width loop   r1:height loop

    VoC_jal(COOLSAND_JpegDmaMcuWithZoom);

    VoC_jal(COOLSAND_JpegBlockWithZoom);

    //ENOCDE BLOCK
    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCY_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_Y1_ADDR,1);
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(y1, LUMINANCE, &dc_y);


    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCU_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cb, CHROMINANCE, &dc_cb);

    VoC_lw16i_set_inc(REG2,GLOBAL_OLD_DCV_ADDR,1);
    VoC_lw16i_set_inc(REG0,JPEGENC_BUFF_CR_ADDR,1);
    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_jal(COOLSAND_encode_block);//encode_block(cr, CHROMINANCE, &dc_cr);

    ////////////////////////////////
    VoC_pop32(REG01,DEFAULT);
    VoC_lw16i_short(REG4,16,IN_PARALLEL);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);


// VoC_directive: PARSER_OFF

//  VoC_NOP();
//  VoC_NOP();
//  VoC_NOP();

//  printf("GlOBAL_ImgOutw:%d\n  ",REGS[0].reg);
// VoC_directive: PARSER_ON

    /////////////////////////////////
    VoC_bnlt16_rd(COOLSAND_JpegEncodeWithZoom_L1,REG0,GlOBAL_ImgOutw);
    VoC_jump(COOLSAND_JpegEncodeWithZoom_L0);
COOLSAND_JpegEncodeWithZoom_L1:

    VoC_lw32_d(RL6,GLOBAL_yScale);

    VoC_shr32_ri(RL6,-3,DEFAULT)
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16i(REG6,Mask_SCALE_BIT);

    VoC_add32_rd(RL7,RL6,GLOBAL_input_p_Y_Colume);

    VoC_and32_rr(RL7,REG67,DEFAULT);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);

    VoC_sw32_d(RL7,GLOBAL_input_p_Y_Colume);
    VoC_sw32_d(RL7,GLOBAL_input_p_UV_Colume);

    VoC_lw16_d(REG3,GlOBAL_ImgInw);

    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_shr16_ri(REG3,-1,IN_PARALLEL);

    VoC_multf32_rd(REG23,REG3,GLOBAL_InputRowsOffset);

    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_add32_rd(REG23,REG23,GLOBAL_DmaInputBufferDddr);//NEW EX_ADDR [BYTE]

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);//height loop
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_sw32_d(REG23,Input_inStreamBufAddr_addr);
    VoC_sw32_d(REG23,GLOBAL_DmaInputBufferDddr);
    VoC_sw32_d(ACC0,GLOBAL_input_p_Y_Row);
    VoC_sw32_d(ACC0,GLOBAL_input_p_UV_Row);



// VoC_directive: PARSER_OFF

//  VoC_NOP();
//  VoC_NOP();

//  printf("GlOBAL_ImgOuth:%d\n ",REGS[1].reg);
// VoC_directive: PARSER_ON


    VoC_bnlt16_rd(COOLSAND_JpegEncodeWithZoom_L2,REG1,GlOBAL_ImgOuth);
    VoC_jump(COOLSAND_JpegEncodeWithZoom_L0);
COOLSAND_JpegEncodeWithZoom_L2:
    ////////////////////////////////////////////////////////////////////
    VoC_jal(COOLSNAD_flush_bits);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}
/**************************************************************************************
 * Function:    COOLSAND_JpegDmaMcuWithZoom
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/14/2012
 **************************************************************************************/
void COOLSAND_JpegDmaMcuWithZoom(void)
{

    VoC_lw32_d(REG67,GLOBAL_xScale);

    VoC_shr32_ri(REG67,-4,DEFAULT);
    VoC_movreg32(REG45,REG67,IN_PARALLEL);

    VoC_sub32_rr(REG45,REG67,REG45,DEFAULT);
    VoC_lw16i_short(REG1,4,IN_PARALLEL);//1(for pixel )+ 2(for uv )+1(for DMA)

    VoC_add32_rd(REG45,REG45,GLOBAL_input_p_Y_Row);

    VoC_add32_rd(REG67,REG67,GLOBAL_input_p_Y_Row);

    VoC_shr32_ri(REG45,PIXEL_SCALE_BIT,DEFAULT);
    VoC_shr32_ri(REG67,PIXEL_SCALE_BIT+1,IN_PARALLEL);

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT);
    VoC_shru32_ri(REG67,-2,IN_PARALLEL);

    VoC_and16_ri(REG4,0xfffe);

    VoC_movreg16(REG2,REG4,DEFAULT);

    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_lw32_d(ACC0,Input_inStreamBufAddr_addr);//[BYTE]

    VoC_sw16_d(REG2,GLOBAL_DmaInputColumes);

    VoC_lw32_d(RL7,GLOBAL_yScale);

    VoC_shr32_ri(RL7,-3,DEFAULT);
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_sub32_rr(REG23,RL7,REG23,DEFAULT);
    VoC_add32_rr(RL6,REG67,ACC0,IN_PARALLEL);//NEW EX_ADDR [BYTE]

    VoC_add32_rd(REG23,REG23,GLOBAL_input_p_Y_Colume);
    VoC_add32_rd(RL7,RL7,GLOBAL_input_p_Y_Colume);

    VoC_shr32_ri(REG23,PIXEL_SCALE_BIT,DEFAULT);
    VoC_shr32_ri(RL7,PIXEL_SCALE_BIT,IN_PARALLEL);

    VoC_sw32_d(RL6,Input_inStreamBufAddr_addr);

    VoC_sw16_d(RL7_LO,GLOBAL_InputRowsOffset);

    VoC_sub16_dr(REG3,GlOBAL_ImgInw,REG4);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG7,DEFAULT);

    VoC_sw16_d(REG3,CFG_DMA_2D_STRIDE);
    VoC_sw16_d(REG2,CFG_DMA_2D_LINES);

    VoC_mult16_rr(REG5,REG4,REG2,DEFAULT);

    VoC_lw16i(REG7,0x1000);

    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_lw16i(REG1,JPEGENC_BUFF_RGB_640_m1_ADDR+640*9+248);//dmai-addr[int]

    VoC_or32_rr(ACC0,REG67,DEFAULT);
    VoC_sub16_rr(REG5,REG1,REG5,IN_PARALLEL);

    VoC_shr16_ri(REG5,1,DEFAULT);
    VoC_shr16_ri(REG4,1,IN_PARALLEL);

    VoC_sw16_d(REG5,GLOBAL_McuInputBuffAddr);

    VoC_sw16_d(REG2,GLOBAL_DmaInputRows);


    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_bnez16_d(COOLSAND_JpegDmaMcuWithZoom_L0,Input_Channel_Num_addr)


    VoC_mult16_rr(REG1,REG4,REG2,DEFAULT);//lines*DMA_SIZE
    VoC_shr16_ri(REG5,-1,IN_PARALLEL);//

    VoC_movreg16(REG0,REG5,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);


    VoC_jal(COOLSAND_RGBToYUV422);

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

COOLSAND_JpegDmaMcuWithZoom_L0:





    VoC_return;
}


/**************************************************************************************
 * Function:    COOLSAND_JpegBlockWithZoom
 *
 * Description: no
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/16/2012
 **************************************************************************************/
void COOLSAND_JpegBlockWithZoom(void)
{


    /************************************************************************/
    /*                              Y  ZOOM in ROW                          */
    /************************************************************************/

    VoC_lw16d_set_inc(REG0,GLOBAL_McuInputBuffAddr,2);//y
    VoC_lw16i_set_inc(REG1,GLOBAL_TempZommBuf,2);

    VoC_lw16i(REG6,Mask_SCALE_BIT);
    VoC_lw16i(REG7,1<<PIXEL_SCALE_BIT);//maxScale

    VoC_lw32_d(RL6,GLOBAL_xScale);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(INC2,2,IN_PARALLEL);

    VoC_lw16_d(REG5,GLOBAL_DmaInputRows);

    VoC_loop_r(1,REG5);

    VoC_lw32_d(RL7,GLOBAL_input_p_Y_Row);

    VoC_loop_i_short(16,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);
    VoC_startloop0

    /*          weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
                weight_A=maxScale-weight_B;
                pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
                wa=(*pxptrY++)*weight_A;
                wa+=(*pxptrY)*weight_B;
                (*ptempY)=(wa>>PIXEL_SCALE_BIT);
                ptempY+=ImgInh;
                input_p+=xScale;*/

    /*      VoC_and16_rr(REG5,REG6,DEFAULT);//weight_B0=input_p&Mask_SCALE_BIT
            VoC_movreg32(REG23,RL7,IN_PARALLEL);

            VoC_add32_rr(RL7,RL7,RL6,DEFAULT);//input_p0+=xScale;
            VoC_shru32_ri(REG23,PIXEL_SCALE_BIT,IN_PARALLEL);//(input_p>>PIXEL_SCALE_BIT);

            VoC_shru16_ri(REG2,-1,DEFAULT);

            VoC_add16_rr(REG2,REG0,REG2,DEFAULT);// pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
            VoC_movreg16(REG3,RL7_LO,IN_PARALLEL);

            VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);//weight_A0=maxScale-weight_B;
            VoC_and16_rr(REG3,REG6,IN_PARALLEL);//weight_B1=input_p&Mask_SCALE_BIT

            VoC_multf32inc_rp(ACC0,REG4,REG2,DEFAULT);

            VoC_mac32_rp(REG5,REG2,DEFAULT);
            VoC_movreg32(REG45,RL7,IN_PARALLEL);

            VoC_sub16_rr(REG2,REG7,REG3,DEFAULT);//weight_A1=maxScale-weight_B;
            VoC_shru32_ri(REG45,PIXEL_SCALE_BIT,IN_PARALLEL);//(input_p1>>PIXEL_SCALE_BIT);

            VoC_shru16_ri(REG4,-1,DEFAULT);

            VoC_add16_rr(REG4,REG0,REG4,DEFAULT);// pxptrY=piY+(input_p1>>PIXEL_SCALE_BIT);
            VoC_movreg32(ACC1,ACC0,IN_PARALLEL);

            VoC_shru32_ri(ACC1,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa0>>PIXEL_SCALE_BIT);

            VoC_multf32inc_rp(ACC0,REG2,REG4,DEFAULT);

            VoC_mac32_rp(REG3,REG4,DEFAULT);

            VoC_add32_rr(RL7,RL7,RL6,DEFAULT);//input_p1+=xScale;

            VoC_shru32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);

            VoC_sw16inc_p(ACC1_LO,REG1,DEFAULT);

            VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
            VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);*/

    VoC_and16_rr(REG5,REG6,DEFAULT);//weight_B0=input_p&Mask_SCALE_BIT
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_shru32_ri(REG23,PIXEL_SCALE_BIT,DEFAULT);//(input_p>>PIXEL_SCALE_BIT);

    VoC_shru16_ri(REG2,-1,DEFAULT);

    VoC_add16_rr(REG2,REG0,REG2,DEFAULT);// pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);//weight_A0=maxScale-weight_B;

    VoC_multf32inc_rp(ACC0,REG4,REG2,DEFAULT);

    VoC_mac32_rp(REG5,REG2,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);
    VoC_add32_rr(RL7,RL7,RL6,IN_PARALLEL);//input_p0+=xScale;

    VoC_NOP();

    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);
    VoC_endloop0

    VoC_add16_rd(REG0,REG0,GLOBAL_DmaInputColumes);

    VoC_endloop1

    VoC_lw16i(REG6,Mask_SCALE_BIT|0x4000);
    VoC_lw16i(REG7,0);

    VoC_and32_rr(RL7,REG67,DEFAULT);

    VoC_NOP();

    VoC_sw32_d(RL7,GLOBAL_input_p_Y_Row);

    /************************************************************************/
    /*                              Y  ZOOM in colume                       */
    /************************************************************************/

    VoC_lw16i_set_inc(REG0,GLOBAL_TempZommBuf,1);//y
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_Y_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_Y1_ADDR,2);

    VoC_lw16i(REG6,Mask_SCALE_BIT);
    VoC_lw16i(REG7,1<<PIXEL_SCALE_BIT);//maxScale

    VoC_lw32_d(RL6,GLOBAL_yScale);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(INC3,2,IN_PARALLEL);

    VoC_movreg16(ACC1_LO,REG1,DEFAULT);
    VoC_movreg16(ACC1_HI,REG2,IN_PARALLEL);

    VoC_lw32_d(RL7,GLOBAL_input_p_Y_Colume);

    VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,-1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-7-PIXEL_SCALE_BIT,DEFAULT);

    VoC_loop_i(1,8);

    VoC_movreg32(REG23,RL7,DEFAULT);
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    VoC_shr32_ri(REG23,PIXEL_SCALE_BIT,DEFAULT);//(input_p>>PIXEL_SCALE_BIT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);

    VoC_and16_rr(REG5,REG6,DEFAULT);//weight_B=input_p&Mask_SCALE_BIT
    VoC_shru16_ri(REG2,-5,IN_PARALLEL);

    VoC_add32_rr(RL7,RL7,RL6,DEFAULT);
    VoC_add16_rr(REG2,REG0,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);//weight_A0=maxScale-weight_B;
    VoC_add16_rr(REG3,REG2,REG4,IN_PARALLEL);


    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg16(REG1,ACC1_LO,IN_PARALLEL);
    VoC_startloop0

    VoC_mac32inc_rp(REG4,REG2,DEFAULT);

    VoC_mac32inc_rp(REG5,REG3,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);

    VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,-1,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_shr32_ri(ACC0,-7-PIXEL_SCALE_BIT,IN_PARALLEL);
    VoC_endloop0

    VoC_movreg16(ACC1_LO,REG1,DEFAULT);

    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg16(REG1,ACC1_HI,IN_PARALLEL);
    VoC_startloop0

    VoC_mac32inc_rp(REG4,REG2,DEFAULT);

    VoC_mac32inc_rp(REG5,REG3,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);

    VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,-1,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_shr32_ri(ACC0,-7-PIXEL_SCALE_BIT,IN_PARALLEL);
    VoC_endloop0

    VoC_movreg16(ACC1_HI,REG1,DEFAULT);

    VoC_endloop1



    /************************************************************************/
    /*                              U V  ZOOM in ROW                        */
    /************************************************************************/

    VoC_lw16d_set_inc(REG0,GLOBAL_McuInputBuffAddr,1);//y
    VoC_lw16i_set_inc(REG1,GLOBAL_TempZommBuf,1);

    VoC_lw16i(REG6,Mask_SCALE_BIT);
    VoC_lw16i(REG7,1<<PIXEL_SCALE_BIT);//maxScale

    VoC_lw32_d(RL6,GLOBAL_xScale);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(INC2,4,IN_PARALLEL);

    VoC_lw16i_short(INC3,4,DEFAULT);
    VoC_inc_p(REG0,IN_PARALLEL);//u

    VoC_lw16_d(REG5,GLOBAL_DmaInputRows);

    VoC_loop_r(1,REG5);


    VoC_lw32_d(RL7,GLOBAL_input_p_UV_Row);

    VoC_loop_i_short(8,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);
    VoC_startloop0

    VoC_and16_rr(REG5,REG6,DEFAULT);//weight_B0=input_p&Mask_SCALE_BIT
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_shru32_ri(REG23,PIXEL_SCALE_BIT,DEFAULT);//(input_p>>PIXEL_SCALE_BIT);

    VoC_shru16_ri(REG2,-2,DEFAULT);

    VoC_add16_rr(REG2,REG0,REG2,DEFAULT);// pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);//weight_A0=maxScale-weight_B;
    VoC_add16_rr(REG3,REG2,REG3,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG4,REG2,DEFAULT);

    VoC_mac32_rp(REG5,REG2,DEFAULT);

    VoC_multf32inc_rp(ACC1,REG4,REG3,DEFAULT);

    VoC_shru32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);
    VoC_mac32_rp(REG5,REG3,IN_PARALLEL);

    VoC_add32_rr(RL7,RL7,RL6,DEFAULT);//input_p0+=xScale;

    VoC_shru32_ri(ACC1,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);

    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_LO,REG1,DEFAULT);

    VoC_endloop0

    VoC_add16_rd(REG0,REG0,GLOBAL_DmaInputColumes);

    VoC_endloop1

    VoC_lw16i(REG6,Mask_SCALE_BIT);
    VoC_lw16i(REG7,0);

    VoC_and32_rr(RL7,REG67,DEFAULT);

    VoC_NOP();

    VoC_sw32_d(RL7,GLOBAL_input_p_UV_Row);



    /************************************************************************/
    /*                             U V  ZOOM in colume                      */
    /************************************************************************/

    VoC_lw16i_set_inc(REG0,GLOBAL_TempZommBuf,1);//y
    VoC_lw16i_set_inc(REG1,JPEGENC_BUFF_CB_ADDR,1);
    VoC_lw16i_set_inc(REG2,JPEGENC_BUFF_CR_ADDR,1);

    VoC_lw16i(REG6,Mask_SCALE_BIT);
    VoC_lw16i(REG7,1<<PIXEL_SCALE_BIT);//maxScale

    VoC_lw32_d(RL6,GLOBAL_yScale);

    VoC_lw16i_short(FORMAT32,0,DEFAULT);
    VoC_lw16i_short(INC3,1,IN_PARALLEL);

    VoC_movreg16(ACC1_LO,REG1,DEFAULT);
    VoC_movreg16(ACC1_HI,REG2,IN_PARALLEL);

    VoC_lw32_d(RL7,GLOBAL_input_p_UV_Colume);


    VoC_push32(ACC1,DEFAULT);

    VoC_lw16i_short(ACC0_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC0_HI,-1,IN_PARALLEL);

    VoC_lw16i_short(ACC1_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,-1,IN_PARALLEL);

    VoC_shr32_ri(ACC0,-7-PIXEL_SCALE_BIT,DEFAULT);
    VoC_shr32_ri(ACC1,-7-PIXEL_SCALE_BIT,IN_PARALLEL);

    VoC_loop_i(1,8);

    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_shru32_ri(REG23,PIXEL_SCALE_BIT,DEFAULT);//(input_p>>PIXEL_SCALE_BIT);
    VoC_movreg16(REG5,REG2,IN_PARALLEL);

    VoC_and16_rr(REG5,REG6,DEFAULT);//weight_B=input_p&Mask_SCALE_BIT
    VoC_shru16_ri(REG2,-4,IN_PARALLEL);

    VoC_add32_rr(RL7,RL7,RL6,DEFAULT);
    VoC_add16_rr(REG2,REG0,REG2,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG7,REG5,DEFAULT);//weight_A0=maxScale-weight_B;
    VoC_add16_rr(REG3,REG2,REG4,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);


    VoC_loop_i_short(8,DEFAULT);

    VoC_startloop0

    VoC_mac32inc_rp(REG4,REG2,DEFAULT);
    VoC_lw16i_short(ACC1_LO,-1,IN_PARALLEL);

    VoC_mac32inc_rp(REG5,REG3,DEFAULT);
    VoC_lw16i_short(ACC1_HI,-1,IN_PARALLEL);

    VoC_shr32_ri(ACC1,-7-PIXEL_SCALE_BIT,DEFAULT);
    VoC_mac32inc_rp(REG4,REG2,IN_PARALLEL);

    VoC_shr32_ri(ACC0,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);
    VoC_mac32inc_rp(REG5,REG3,IN_PARALLEL);

    VoC_NOP();

    VoC_shr32_ri(ACC1,PIXEL_SCALE_BIT,DEFAULT);//(*ptempY)=(wa1>>PIXEL_SCALE_BIT);
    VoC_lw16i_short(ACC0_LO,-1,IN_PARALLEL);

    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);//CB
    VoC_lw16i_short(ACC0_HI,-1,IN_PARALLEL);

    VoC_sw16inc_p(ACC1_LO,REG1,DEFAULT);//CR
    VoC_shr32_ri(ACC0,-7-PIXEL_SCALE_BIT,IN_PARALLEL);


    VoC_endloop0

    VoC_pop16(REG0,DEFAULT);
    VoC_push32(REG01,IN_PARALLEL);

    VoC_endloop1

    VoC_pop32(REG01,DEFAULT);

    VoC_return;
}

/**************************************************************************************
 * Function:    COOLSAND_RGBToYUV422
 *
 * Description: no
 *
 * Inputs:
 *
 *          REG0:rgb buf
 *          REG1:int
 * Outputs:
 *          REG2:yuv buf
 *
 * Used : R0 R1 R2 R3 R4 R5 R6 R7 ACC0 ACC1.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       06/05/2012
 **************************************************************************************/
void COOLSAND_RGBToYUV422(void)
{


    /*

        Y=0.2990R+0.5870G+0.1140B
        U=-0.1687R-0.3313G+0.5000B+128
        V=0.5000R-0.4187G-0.0813B＋128

        Y=(1225*R+2404*G+ 467*B)          >>12
        U=(-691*R-1357*G+2048*B+2048*255) >>12
        V=( 2048R-1715*G- 333*B+2048*255) >>12

    */

    VoC_movreg16(RL7_LO,REG0,DEFAULT);

    VoC_lw16i_short(INC0,2,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16i(RL6_LO,0xf800)

    VoC_lw16i_short(RL6_HI,0x7,DEFAULT);//RL6:255*2048=0x7f800;
    VoC_movreg16(REG7,REG1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,GLOBAL_RGBToYUV422Coef,2);

    VoC_lw32z(ACC0,DEFAULT);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);

COOLSAND_RGBToYUV422_L0:

    VoC_lw32inc_p(REG23,REG0,DEFAULT);
    VoC_movreg16(RL7_HI,REG7,IN_PARALLEL);

    VoC_shru16_ri(REG3,-8,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_or16_rr(REG3,REG2,DEFAULT);
    VoC_shru16_ri(REG4,3,IN_PARALLEL);

    VoC_shru16_ri(REG3,-5,DEFAULT);
    VoC_shru16_ri(REG2,-11,IN_PARALLEL);

    VoC_shru16_ri(REG3,10,DEFAULT);
    VoC_shru16_ri(REG2,8,IN_PARALLEL);//B

    VoC_shru16_ri(REG3,-2,DEFAULT);//G
    VoC_shru16_ri(REG4,-3,IN_PARALLEL);//R

    //Y=(1225*R+2404*G+ 467*B)        >>12
    //U=(-691*R-1357*G+2048*B+2048*255) >>12

    VoC_multf32inc_rp(ACC0,REG4,REG1,DEFAULT);
    VoC_mac32inc_rp(REG4,REG1,IN_PARALLEL);

    VoC_mac32inc_rp(REG3,REG1,DEFAULT);
    VoC_mac32inc_rp(REG3,REG1,IN_PARALLEL);

    VoC_mac32inc_rp(REG2,REG1,DEFAULT);
    VoC_mac32inc_rp(REG2,REG1,IN_PARALLEL);

    VoC_lw32_p(REG67,REG0,DEFAULT);
    VoC_movreg16(REG0,RL7_LO,IN_PARALLEL);

    VoC_shru32_ri(ACC0,12,DEFAULT);//y
    VoC_shru32_ri(ACC1,12,IN_PARALLEL);//u

    VoC_shru16_ri(REG7,-8,DEFAULT);
    VoC_movreg16(REG5,REG7,IN_PARALLEL);

    VoC_or16_rr(REG7,REG6,DEFAULT);
    VoC_shru16_ri(REG5,3,IN_PARALLEL);

    VoC_shru16_ri(REG7,-5,DEFAULT);
    VoC_shru16_ri(REG6,-11,IN_PARALLEL);

    VoC_shru16_ri(REG7,10,DEFAULT);
    VoC_shru16_ri(REG6,8,IN_PARALLEL);//B

    VoC_shru32_ri(ACC1,-16,DEFAULT);
    VoC_shru16_ri(REG5,-3,IN_PARALLEL);//R

    VoC_or32_rr(ACC0,ACC1,DEFAULT);
    VoC_movreg32(ACC1,RL6,IN_PARALLEL);

    //Y=(1225*R+2404*G+ 467*B)        >>12
    //V=( 2048R-1715*G- 333*B+2048*255) >>12

    VoC_multf32inc_rp(ACC0,REG5,REG1,DEFAULT);
    VoC_mac32inc_rp(REG4,REG1,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_shru16_ri(REG7,-2,IN_PARALLEL);//G

    VoC_mac32inc_rp(REG7,REG1,DEFAULT);
    VoC_mac32inc_rp(REG3,REG1,IN_PARALLEL);

    VoC_mac32inc_rp(REG6,REG1,DEFAULT);
    VoC_mac32inc_rp(REG2,REG1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,GLOBAL_RGBToYUV422Coef,2);

    VoC_shru32_ri(ACC0,12,DEFAULT);
    VoC_shru32_ri(ACC1,12,IN_PARALLEL);

    VoC_shru32_ri(ACC1,-16,DEFAULT);
    VoC_movreg16(REG7,RL7_HI,IN_PARALLEL);

    VoC_or32_rr(ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_movreg32(ACC1,RL6,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_movreg16(RL7_LO,REG0,IN_PARALLEL);


    VoC_bgtz16_r(COOLSAND_RGBToYUV422_L0,REG7)


    VoC_return;
}